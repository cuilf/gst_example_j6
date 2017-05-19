/*************************************************************************
    > File Name: rtpsend.c
    > Author: cuilf
    > Mail: cuilongfeiii@163.com 
    > Created Time: Thu 20 Apr 2017 06:06:59 PM CST
 ************************************************************************/

#include <stdio.h>
#include <gst/gst.h>
#include <gst/gstcaps.h>
#include <gst/gstelement.h>
#include <glib.h>


//gst-launch-1.0 -e v4l2src device=/dev/video1 ! 'video/x-raw, format=(string)UYVY' ! videoconvert ! 'video/x-raw, format=(string)NV12' !  ducatih264enc bitrate=4000 ! queue ! h264parse ! rtph264pay pt=96  ! udpsink host=192.168.1.101 port=5000
int
main (int argc, char *argv[])
{

	GstElement *pipeline, *video_source, *video_convert, *video_enc, *queue, *video_parse, *rtp_pay, *udp_sink;
	GstBus *bus;  
    GstMessage *msg;
	GstCaps *caps_1, *caps_2;

	
	gst_init (&argc, &argv);
	
	video_source = gst_element_factory_make ("v4l2src", "video_source");
	video_convert = gst_element_factory_make ("videoconvert", "video_convert");
	video_enc = gst_element_factory_make ("ducatih264enc", "video_enc");
	queue = gst_element_factory_make ("queue", "queue");
	video_parse = gst_element_factory_make ("h264parse", "video_parse");
	rtp_pay = gst_element_factory_make ("rtph264pay", "rtp_pay");
	udp_sink = gst_element_factory_make ("udpsink", "udpsink_sink");
	
	pipeline = gst_pipeline_new ("test_pipeline");
	
	if (!pipeline || !video_source || !video_convert || !video_enc || !queue || !video_parse || !rtp_pay || !udp_sink)
	{
		g_printerr ("Not all element could be created!\n");
		return -1;
	}
	
	g_object_set (video_source, "device", "/dev/video1", NULL);
	g_object_set (rtp_pay, "pt", 96, NULL);
	g_object_set (video_enc, "bitrate", 4000, NULL);
	g_object_set (udp_sink, "host", "192.168.2.101", "port", 5000, NULL);

	gst_bin_add_many (GST_BIN(pipeline), video_source, video_convert, video_enc, queue, video_parse, rtp_pay, udp_sink, NULL);	
	
	if (gst_element_link_many (video_enc, queue, video_parse, rtp_pay, udp_sink, NULL) != TRUE)
	{
		g_printerr ("Element could not be linked\n");
		gst_object_unref (pipeline);
		return -1;
	}

	caps_1 = gst_caps_new_simple ("video/x-raw",
          "format", G_TYPE_STRING, "UYVY",
          NULL);
	caps_2 = gst_caps_new_simple ("video/x-raw",
          "format", G_TYPE_STRING, "NV12",
          NULL);


	if (!gst_element_link_filtered (video_source, video_convert, caps_1))
	{
		g_printerr ("Failed to link video_source and video_convert!\n");
		return -1;
	}

	if (!gst_element_link_filtered (video_convert, video_enc, caps_2))
	{
		g_printerr ("Failed to link video_source and video_convert!\n");
		return -1;
	}
	gst_caps_unref (caps_1);
	gst_caps_unref (caps_2);
	
	
	gst_element_set_state (pipeline, GST_STATE_PLAYING); 
	
		
	bus = gst_element_get_bus (pipeline);  
	
	
	msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS); 
	 
	

	if (msg != NULL)  
		gst_message_unref (msg);  
	gst_object_unref (bus);  
	gst_element_set_state (pipeline, GST_STATE_NULL);  

	gst_object_unref (pipeline);
	
	return 0;
}
