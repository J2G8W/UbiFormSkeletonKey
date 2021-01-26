package com.example.ubiformskeletonkey

import android.app.Service
import android.content.Intent
import android.net.wifi.WifiManager
import android.os.Binder
import android.os.IBinder

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
    external fun addRDH(url: String): Boolean
    external fun getRDHUrls(): Array<String>

    external fun updateManifestWithHubs()
    external fun deregisterFromAllHubs()
    external fun closeRDH()
    external fun openRDH()


    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}