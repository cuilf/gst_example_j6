/*************************************************************************
    > File Name: capture.c
    > Author: cuilf
    > Mail: cuilongfeiii@163.com 
    > Created Time: Thu 20 Apr 2017 06:06:59 PM CST
 ************************************************************************/

#include <stdio.h>
#include <gst/gst.h>
#include <gst/gstcaps.h>
#include <gst/gstelement.h>
#include <glib.h>
#include <pthread.h>


//gst-launch-1.0 -e v4l2src device=/dev/video1 ! 'video/x-raw, format=(string)UYVY' ! videoconvert ! 'video/x-raw, format=(string)NV12' !  ducatih264enc bitrate=4000 ! queue ! h264parse ! qtmux ! filesink location=BBB.mp4

//gst-launch-1.0 -e v4l2src device=/dev/video1 io-mode=4 ! videoconvert ! 'video/x-raw, format=(string)YUY2' ! vpe ! queue ! ducatih264enc level=41 profile=66 bitrate=4000 slice-mode=1 rate-preset=low-delay rate-control-params-preset=2  intra-interval=2 inter-interval=1 ! h264parse !  mpegtsmux ! queue ! filesink location=x.ts &

void *handler_end (void *arg)
{
	GstElement *pipeline = (GstElement *)arg;

	sleep (10);
		gst_element_send_event(pipeline, gst_event_new_eos());
	
	pthread_exit(0);
}

int
main (int argc, char *argv[])
{

	GstElement *pipeline, *video_source, *video_convert, *vpe, *video_enc, *queue, *video_parse, *qtmux, *file_sink;
	GstBus *bus;  
    GstMessage *msg;
	GstCaps  *caps_2;

	pthread_t thread_end;

	
	gst_init (&argc, &argv);
	
	video_source = gst_element_factory_make ("v4l2src", "video_source");
	video_convert = gst_element_factory_make ("videoconvert", "video_convert");
	vpe = gst_element_factory_make ("vpe", "vpe");
	video_enc = gst_element_factory_make ("ducatih264enc", "video_enc");
	queue = gst_element_factory_make ("queue", "queue");
	video_parse = gst_element_factory_make ("h264parse", "video_parse");
	qtmux = gst_element_factory_make ("qtmux", "qtmux");
	file_sink= gst_element_factory_make ("filesink", "file_sink");
	
	pipeline = gst_pipeline_new ("test_pipeline");
	
	if (!pipeline || !video_source || !video_convert || !vpe || !video_enc || !queue || !video_parse || !qtmux || !file_sink)
	{
		g_printerr ("Not all elemnt could be created!\n");
		return -1;
	}
	
	g_object_set (video_source, "device", "/dev/video1", "io-mode", 4, NULL);
	g_object_set (video_enc, "bitrate", 4000, NULL);
	g_object_set (file_sink, "location", argv[1], NULL);

	gst_bin_add_many (GST_BIN(pipeline), video_source, video_convert, vpe, video_enc, queue, video_parse, qtmux, file_sink, NULL);	
	
	if ((gst_element_link_many (video_source, video_convert, NULL) != TRUE) ||
		(gst_element_link_many (vpe, video_enc, queue, video_parse, qtmux, file_sink, NULL) != TRUE))
	{
		g_printerr ("Element could not be linked\n");
		gst_object_unref (pipeline);
		return -1;
	}


	caps_2 = gst_caps_new_simple ("video/x-raw",
          "format", G_TYPE_STRING, "YUY2",
          NULL);



	if (!gst_element_link_filtered (video_convert, vpe, caps_2))
	{
		g_printerr ("Failed to link video_source and video_convert!\n");
		return -1;
	}
	
	gst_caps_unref (caps_2);
	
	
	gst_element_set_state (pipeline, GST_STATE_PLAYING); 
	
	pthread_create (&thread_end, NULL, handler_end, pipeline);
	bus = gst_element_get_bus (pipeline);  
	
	
	msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS); 
	 
	

	if (msg != NULL)  
	{
		GError *err;
		gchar *debug_info;

		switch (GST_MESSAGE_TYPE (msg)) 
		{
		  case GST_MESSAGE_ERROR:
			gst_message_parse_error (msg, &err, &debug_info);
			g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
			g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
			g_clear_error (&err);
			g_free (debug_info);
			break;
		  case GST_MESSAGE_EOS:
			g_print ("End-Of-Stream reached.\n");
			break;
		  default:
			/* We should not reach here because we only asked for ERRORs and EOS */
			g_printerr ("Unexpected message received.\n");
			break;
		}
		gst_message_unref (msg); 
	}
	 
	gst_object_unref (bus);  
	gst_element_set_state (pipeline, GST_STATE_NULL);  

	gst_object_unref (pipeline);
	
	return 0;
}
