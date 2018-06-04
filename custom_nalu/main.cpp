#include <gst/gst.h>
#include <gst/app/app.h>
#include <glib.h>
#include <gst/codecparsers/gsth264parser.h>

#include <QDebug>
#include <iostream>

GMainLoop *loop;

static void on_new_sample (GstElement *el, void *data) {
    GstSample *sample = NULL;
    g_signal_emit_by_name (el, "pull-sample", &sample);
    if (!sample) {
        g_printerr("no smaple!");
        return;
    }

    GstCaps *caps = gst_sample_get_caps(sample);
    g_print(gst_caps_to_string(caps));

    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstMapInfo map;
    gst_buffer_map(buffer, &map, GST_MAP_READ);

    GstH264NalUnit nalu;
    GstH264NalParser *parser = gst_h264_nal_parser_new ();
    GstH264ParserResult res = gst_h264_parser_identify_nalu(parser, map.data, 0, map.size, &nalu);
    if(GST_H264_PARSER_OK == res) {
        g_print("NALU TYPE %0x LEN %i", nalu.type, nalu.size);
        if(0X0c == nalu.type) {
            printf("\n NALU HEX DUMP -------------------------------------\n");
            for (int i = 0; i < nalu.size; i++) {
                printf("%02X ", nalu.data[i]);
            }
            printf("\n-------------------------------------\n");
        }
    }
    fflush(stdout);
    fflush(stderr);

    gst_h264_nal_parser_free (parser);
    gst_buffer_unmap (buffer, &map);
    gst_sample_unref (sample);
}

int main (int argc, char *argv[]) {
    gst_init (&argc, &argv);
    loop = g_main_loop_new (NULL, FALSE);

    GError *err = NULL;
    gchar descr[] =
            "rtspsrc location=rtsp://grubba.no-ip.org:8554/live ! "
            "queue ! "
            "rtph264depay ! "
            "h264parse ! "
            "video/x-h264, parsed=true, stream-format=byte-stream, alignment=au ! "
            "appsink name=appsink drop=true emit-signals=true"
            ;
    GstElement *pipeline = gst_parse_launch(descr, &err);
    if(err) {
        g_printerr("PARSE ERR: %i %i %s", err->code, err->domain, err->message);
        return -1;
    }

    GstElement *appsink = gst_bin_get_by_name(GST_BIN(bin), "appsink");
    g_signal_connect (appsink, "new-sample", G_CALLBACK (on_new_sample), NULL);

    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    g_main_loop_run (loop);
    gst_element_set_state (pipeline, GST_STATE_NULL);

    gst_object_unref (GST_OBJECT (pipeline));
    g_main_loop_unref (loop);

    return 0;
}
