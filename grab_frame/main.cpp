#include <gst/gst.h>
#include <gst/app/app.h>
#include <glib.h>
#include <gst/codecparsers/gsth264parser.h>

#include <QImage>
#include <iostream>

GMainLoop *loop;

static void on_new_sample_color (GstElement *el, void *data) {
    GstSample *sample;
    g_signal_emit_by_name (el, "pull-sample", &sample);
    if (!sample) {
        g_printerr("no smaple!");
        return;
    }

    GstCaps * caps = gst_sample_get_caps (sample);
    GstStructure *capsStruct = gst_caps_get_structure(caps,0);
    int width = 0, height = 0;
    gst_structure_get_int(capsStruct, "width", &width);
    gst_structure_get_int(capsStruct, "height", &height);
    if( 0 == width || 0 == height) {
        gst_sample_unref(sample);
        return;
    }
    QImage img(width, height, QImage::Format_RGB888);

    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstMapInfo map;
    gst_buffer_map(buffer, &map, GST_MAP_READ);
    memcpy(img.scanLine(0), map.data, map.size);
    gst_buffer_unmap (buffer, &map);

    gst_sample_unref(sample);

    img.save("color.png");

    g_main_loop_quit(loop);
}

static void on_new_sample_gray (GstElement *el, void *data) {
    GstSample *sample;
    g_signal_emit_by_name (el, "pull-sample", &sample);
    if (!sample) {
        g_printerr("no smaple!");
        return;
    }

    GstCaps * caps = gst_sample_get_caps (sample);
    GstStructure *capsStruct = gst_caps_get_structure(caps,0);
    int width = 0, height = 0;
    gst_structure_get_int(capsStruct, "width", &width);
    gst_structure_get_int(capsStruct, "height", &height);
    if( 0 == width || 0 == height) {
        gst_sample_unref(sample);
        return;
    }
    QImage img(width, height, QImage::Format_Grayscale8);

    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstMapInfo map;
    gst_buffer_map(buffer, &map, GST_MAP_READ);
    memcpy(img.scanLine(0), map.data, map.size);
    gst_buffer_unmap (buffer, &map);

    gst_sample_unref(sample);

    img.save("gray.png");

    g_main_loop_quit(loop);
}

int main (int argc, char *argv[]) {
    gst_init (&argc, &argv);
    loop = g_main_loop_new (NULL, FALSE);

    GError *err = NULL;
    gchar descr[] =
            "videotestsrc is-live=true ! "
            "tee name=t ! "
            "queue ! "
            "videoconvert ! video/x-raw, format=RGB ! "
            "appsink name=app_sink_color drop=true  emit-signals=true "
            "t. ! "
            "queue ! "
            "videoconvert ! video/x-raw, format=GRAY8 ! "
            "appsink name=app_sink_gray drop=true emit-signals=true "
            ;
    GstElement *pipeline = gst_parse_launch(descr, &err);
    if(err) {
        g_printerr("PARSE ERR: %i %i %s", err->code, err->domain, err->message);
        return -1;
    }

    GstElement *appsink_color = gst_bin_get_by_name(GST_BIN(pipeline), "app_sink_color");
    g_signal_connect (appsink_color, "new-sample", G_CALLBACK (on_new_sample_color), NULL);
    gst_object_unref(appsink_color);

    GstElement *appsink_gray = gst_bin_get_by_name(GST_BIN(pipeline), "app_sink_gray");
    g_signal_connect (appsink_gray, "new-sample", G_CALLBACK (on_new_sample_gray), NULL);
    gst_object_unref(appsink_gray);

    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    g_main_loop_run (loop);
    gst_element_set_state (pipeline, GST_STATE_NULL);

    gst_object_unref (GST_OBJECT (pipeline));
    g_main_loop_unref (loop);

    return 0;
}
