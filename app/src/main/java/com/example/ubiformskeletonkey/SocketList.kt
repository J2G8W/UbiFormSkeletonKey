package com.example.ubiformskeletonkey

import android.os.Bundle
import android.widget.TextView

class SocketList : GeneralConnectedActivity() {
    var rdhUrl : String = ""
    var componentId: String = ""
    var successfulConnection: Boolean = false
    var correctComponentUrl: String = ""

    override fun connectedToUbiForm() {
        correctComponentUrl = mService.getCorrectRemoteAddress(rdhUrl, componentId,findViewById(R.id.main_output))
        successfulConnection = correctComponentUrl.startsWith("tcp")

        if(successfulConnection) {
            findViewById<TextView>(R.id.main_output).text = "Connected to ${correctComponentUrl}"
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_socket_list)
        rdhUrl = intent.getStringExtra("rdh").toString()
        componentId = intent.getStringExtra("id").toString()
    }


}