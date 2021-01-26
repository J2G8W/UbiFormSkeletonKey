package com.example.ubiformskeletonkey

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView

class SocketList : GeneralConnectedActivity() {
    var rdhUrl : String = ""
    var componentId: String = ""
    var successfulConnection: Boolean = false
    var correctComponentUrl: String = ""

    override fun connectedToUbiForm() {
        correctComponentUrl = mService.getCorrectRemoteAddress(rdhUrl,componentId)
        successfulConnection = correctComponentUrl.startsWith("tcp")

        if(successfulConnection) {
            findViewById<TextView>(R.id.main_output).text = "Connected to ${correctComponentUrl}"
        }else{
            findViewById<TextView>(R.id.main_output).text = correctComponentUrl
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_socket_list)
        rdhUrl = intent.getStringExtra("rdh").toString()
        componentId = intent.getStringExtra("id").toString()
    }


}