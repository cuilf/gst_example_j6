/*************************************************************************
    > File Name: capture-v2.c
    > Author: cuilf
    > Mail: cuilongfeiii@163.com 
    > Created Time: Thu 18 May 2017 03:55:05 PM CST
 ************************************************************************/

#include <stdio.h>
#include <gst/gst.h>
#include <gst/gstcaps.h>
#include <gst/gstelement.h>
#include <glib.h>

//gst-launch-1.0 -e videotestsrc pattern=18 ! 'video/x-raw, format=(string)NV12, height=(int)720, width=(int)1280' ! \
clockoverlay halignment=2 valignment=2 font-desc="Sans Bold 12" time-format="%Y/%m/%d %H:%M:%S" ! \
timeoverlay halignment=2 valignment=1 font-desc="Sans Bold 12" !  \
ducatih264enc bitrate=4000 slice-mode=1 rate-preset=1 rate-control-params-preset=2  intra-interval=2 inter-interval=1 ! \
multifilesink location=VIDEO%04d.264 next-file=5  max-file-duration=180000000000



int main (int argc, char* argv[])
{
	GstElement *pipeline, *video_source, *video_filter, *clockoverlay, *timeoverlay, *video_enc, *queue, *video_parse, *qtmux, *multifile_sink;
	GstBus *bus;  
    GstMessage *msg;
	GstCaps  *srccap;


	
	gst_init (&argc, &argv);
	
	video_source = gst_element_factory_make ("videotestsrc", "video_source");
	video_filter = gst_element_factory_make ("capsfilter", "video_filter");
	//vpe = gst_element_factory_make ("vpe", "vpe");
	video_enc = gst_element_factory_make ("ducatih264enc", "video_enc");
	queue = gst_element_factory_make ("queue", "queue");
	video_parse = gst_element_factory_make ("h264parse", "video_parse");
	qtmux = gst_element_factory_make ("qtmux", "qtmux");
	multifile_sink= gst_element_factory_make ("multifilesink", "multifile_sink");
	clockoverlay = gst_element_factory_make ("clockoverlay", "clockoverlay");
	timeoverlay = gst_element_factory_make ("timeoverlay", "timeoverlay");


	pipeline = gst_pipeline_new ("test_pipeline");
	
	if (!pipeline || !video_source || !video_filter || !clockoverlay || !timeoverlay || !video_enc || !multifile_sink)
	{
		g_printerr ("Not all elemnt could be created!\n");
		return -1;
	}
	
	g_object_set (video_source, "pattern", 18, NULL);
	
	srccap = gst_caps_new_simple ("video/x-raw",
		"format", G_TYPE_STRING, "NV12",
		"width", G_TYPE_INT, 1280,
		"height", G_TYPE_INT, 720,
		NULL);
	g_object_set (video_filter, "caps", srccap, NULL);

	g_object_set (clockoverlay, "halignment", 2, "valignment", 2, 
		"font-desc", "Sans Bold 12", "time-format", "%Y/%m/%d %H:%M:%S", NULL);
	g_object_set (timeoverlay, "halignment", 2, "valignment", 1,
		"font-desc", "Sans Bold 12", NULL);

	g_object_set (video_enc, "bitrate", 4000, "slice-mode", 1, "rate-preset", 1,
		"rate-control-params-preset", 2, "intra-interval", 2, "inter-interval", 1, NULL);

	g_object_set (multifile_sink, "location", "REC%04d.264", "next-file", 5, 
		"max-file-duration", 180000000000, NULL);

	gst_bin_add_many (GST_BIN(pipeline), video_source, video_filter, clockoverlay, timeoverlay, video_enc, multifile_sink, NULL);	
	
	if ((gst_element_link_many (video_source, video_filter, clockoverlay, timeoverlay, video_enc, multifile_sink, NULL) != TRUE))
	{
		g_printerr ("Element could not be linked\n");
		gst_object_unref (pipeline);
		return -1;
	}




	gst_element_set_state (pipeline, GST_STATE_PLAYING); 
	
	
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


