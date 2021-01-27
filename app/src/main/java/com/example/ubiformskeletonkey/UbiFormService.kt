package com.example.ubiformskeletonkey

import android.app.Service
import android.content.Intent
import android.net.wifi.WifiManager
import android.os.Binder
import android.os.IBinder
import android.widget.TextView

class UbiFormService : Service() {
    private val binder = LocalBinder()

    override fun onCreate(){
        val wifiManager = applicationContext.getSystemService(WIFI_SERVICE) as WifiManager
        val numAddress = wifiManager.connectionInfo.ipAddress.toLong()
        val ipAddress = "tcp://${(numAddress and 0xff)}.${(numAddress shr 8 and 0xff)}.${(numAddress shr 16 and 0xff)}.${(numAddress shr 24 and 0xff)}"

        startComponent(ipAddress)
        startRDH()
    }

    override fun onBind(intent: Intent): IBinder {
        return binder
    }


    override fun onDestroy() {
        super.onDestroy()
        endComponent()
    }

    inner class LocalBinder : Binder() {
        // Return this instance of LocalService so clients can call public methods
        fun getService(): UbiFormService = this@UbiFormService
    }

    external fun startComponent(ipAddress: String) : String
    external fun startRDH() : String

    external fun getComponentAddress() : String
    external fun getRdhAddress() : String

    external fun endComponent() : Unit
    external fun addRDH(url: String, errorTextObject: TextView): Boolean
    external fun getRDHUrls(): Array<String>

    external fun updateManifestWithHubs()
    external fun deregisterFromAllHubs()
    external fun closeRDH()
    external fun openRDH()

    external fun getComponents(rdhUrl: String, errorTextObject: TextView): Array<String>

    external fun getCorrectRemoteAddress(rdhUrl: String, componentId: String, errorTextObject: TextView) : String

    external fun requestCloseSocketsOfType(url:String, endpointType:String, errorTextObject: TextView) :Boolean
    external fun requestCreateRDH(url: String, errorTextObject: TextView) :Boolean
    external fun requestCloseRDH(url: String,errorTextObject: TextView) :Boolean
    external fun requestAddRDH(url:String, rdh:String,errorTextObject: TextView) :Boolean
    external fun requestRemoveRDH(url:String, rdh: String,errorTextObject: TextView) :Boolean


    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}