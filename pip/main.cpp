#include <gst/gst.h>
#include <glib.h>

#include <QImage>
#include <iostream>

#include <QApplication>
#include <QSlider>

int main (int argc, char *argv[]) {
    gst_init (&argc, &argv);

    GError *err = NULL;
    gchar descr[] =
            "videotestsrc is-live=true ! "
            "videobox name=vbox0 top=-50 bottom=-50 left=-50 right=-50 ! "
            "videoconvert ! "
            "queue ! "

            "videomixer name=mix background=black "
            "sink_0::alpha=0.3 "
            "sink_1::alpha=0.7 "
            " ! "
            "autovideosink "

            "videotestsrc is-live=true pattern=pinwheel ! "
            "videobox name=vbox1 top=0 bottom=0 left=0 right=0 ! "
            "videoconvert ! "
            "queue ! "

            "mix. "
            ;
    GstElement *pipeline = gst_parse_launch(descr, &err);
    if(err) {
        g_printerr("PARSE ERR: %i %i %s", err->code, err->domain, err->message);
        return -1;
    }

//    GstElement *appsink_color = gst_bin_get_by_name(GST_BIN(pipeline), "app_sink_color");

    gst_element_set_state (pipeline, GST_STATE_PLAYING);

//    g_main_loop_run (loop);

    QApplication app(argc, argv);
    QSlider s;
    QObject::connect(&s, &QSlider::valueChanged, [=](int value){
        GstElement *el = gst_bin_get_by_name(GST_BIN(pipeline), "mix");
        GstPad *pad = gst_element_get_static_pad(el, "sink_1");
        g_object_set(pad, "alpha", value / 100.0, NULL);
        gst_object_unref (pad);
        gst_object_unref (el);
    });
    s.show();
    app.exec();

    gst_element_set_state (pipeline, GST_STATE_NULL);

    gst_object_unref (GST_OBJECT (pipeline));

   //this is simple test for git

    return 0;
}
