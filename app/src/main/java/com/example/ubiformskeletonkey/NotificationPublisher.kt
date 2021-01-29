package com.example.ubiformskeletonkey

import android.app.Notification.EXTRA_TEXT
import android.app.Notification.EXTRA_TITLE
import android.app.Service
import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.os.Binder
import android.os.IBinder
import android.service.notification.NotificationListenerService
import android.service.notification.StatusBarNotification
import android.util.Log

class NotificationPublisher : NotificationListenerService() {
    private lateinit var ubiFormService: UbiFormService
    private var ubiformServiceBound: Boolean = false
    private val ubiformServiceConnection = object : ServiceConnection {
        override fun onServiceConnected(className: ComponentName, service: IBinder) {
            // We've bound to LocalService, cast the IBinder and get LocalService instance
            val binder = service as UbiFormService.LocalBinder
            ubiFormService = binder.getService()
            ubiformServiceBound = true
        }

        override fun onServiceDisconnected(arg0: ComponentName) {
            ubiformServiceBound = false
        }
    }

    override fun onNotificationPosted(sbn: StatusBarNotification?) {
        if (ubiformServiceBound) {
            if (sbn != null) {
                ubiFormService.publishNotification(
                    sbn.notification.extras.getString(EXTRA_TITLE),
                    sbn.notification.extras.getString(EXTRA_TEXT)
                )
            }
        }else{
            Log.e("NOTIFICATION","Service not bounded")
        }
    }

    override fun onCreate() {
        super.onCreate()
        Intent(this, UbiFormService::class.java).also { intent ->
            bindService(intent, ubiformServiceConnection, Context.BIND_AUTO_CREATE)
        }
    }


    override fun onDestroy() {
        super.onDestroy()
        ubiformServiceBound = false
        unbindService(ubiformServiceConnection)
    }
}