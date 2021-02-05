package com.example.ubiformskeletonkey

import android.app.Service
import android.content.Intent
import android.net.wifi.WifiManager
import android.os.Binder
import android.os.IBinder
import android.util.Log

class UbiFormService : Service() {
    private val binder = LocalBinder()

    override fun onCreate() {
        val wifiManager = applicationContext.getSystemService(WIFI_SERVICE) as WifiManager
        val numAddress = wifiManager.connectionInfo.ipAddress.toLong()
        val ipAddress =
            "tcp://${(numAddress and 0xff)}.${(numAddress shr 8 and 0xff)}.${(numAddress shr 16 and 0xff)}.${(numAddress shr 24 and 0xff)}"

        val componentMsg = startComponent(ipAddress)
        if (componentMsg.isNotEmpty()) {
            Log.e("Component Creation", componentMsg)
        }
        val rdhMsg = startRDH()
        if (rdhMsg.isNotEmpty()) {
            Log.e("RDH Creation", rdhMsg)
        }
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

    external fun startComponent(ipAddress: String): String
    external fun startRDH(): String

    external fun getComponentAddress(): String
    external fun getRdhAddress(): String

    external fun endComponent(): Unit
    external fun addRDH(url: String, activityObject: GeneralConnectedActivity): Boolean
    external fun getRDHUrls(): Array<String>

    external fun updateManifestWithHubs(activityObject: GeneralConnectedActivity)
    external fun deregisterFromAllHubs(activityObject: GeneralConnectedActivity)
    external fun closeRDH(activityObject: GeneralConnectedActivity)
    external fun openRDH(activityObject: GeneralConnectedActivity)

    external fun getComponentsFromRDH(
        rdhUrl: String,
        activityObject: GeneralConnectedActivity
    ): Array<String>

    external fun getCorrectRemoteAddress(
        rdhUrl: String,
        componentId: String,
        activityObject: GeneralConnectedActivity
    ): String

    external fun getSocketDescriptors(
        url: String,
        activityObject: GeneralConnectedActivity
    ): Array<String>

    external fun requestCloseSocketsOfType(
        url: String,
        endpointType: String,
        activityObject: GeneralConnectedActivity
    )

    external fun requestCreateRDH(url: String, activityObject: GeneralConnectedActivity)
    external fun requestCloseRDH(url: String, activityObject: GeneralConnectedActivity)
    external fun requestAddRDH(url: String, rdh: String, activityObject: GeneralConnectedActivity)
    external fun requestRemoveRDH(
        url: String,
        rdh: String,
        activityObject: GeneralConnectedActivity
    )

    external fun requestComponentManifest(
        url: String,
        activityObject: GeneralConnectedActivity
    ): String

    external fun requestChangeComponentManifest(
        url: String,
        manifest: String,
        activityObject: GeneralConnectedActivity
    )

    external fun publishNotification(title: String?, extraTest: String?, iconImage : ByteArray?)
    external fun requestCloseSocketsOfID(correctComponentUrl: String, socketId: String, activityObject: GeneralConnectedActivity)


    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}