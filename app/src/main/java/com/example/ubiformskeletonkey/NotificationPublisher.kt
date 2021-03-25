package com.example.ubiformskeletonkey

import android.app.Notification.EXTRA_TEXT
import android.app.Notification.EXTRA_TITLE
import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
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
        Log.d("NOTICICATION", "Received")

        if (ubiformServiceBound) {
            if (sbn != null) {
                /*
                var byteArray: ByteArray? = null
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                    try {
                        val remotePackageContext = this.createPackageContext(
                            sbn.packageName, 0
                        )


                        val icon = sbn.notification.smallIcon.loadDrawable(remotePackageContext)

                        if (icon != null) {
                            val bitmap = icon.toBitmap(
                                icon.intrinsicWidth,
                                icon.intrinsicHeight,
                                Bitmap.Config.ARGB_8888
                            )
                            val stream = ByteArrayOutputStream()
                            bitmap.compress(Bitmap.CompressFormat.PNG, 100, stream)
                            byteArray = Base64.encode(stream.toByteArray(), Base64.NO_WRAP)
                        }

                    } catch (e: Exception) {
                        Log.e("NOTIFICATION", e.toString())
                        // DO NOTHING SIMPLY GIVE NO ICON
                    }
                }*/

                Log.d("NOTIFICATION", "IT5")

                val title = sbn.notification.extras.getCharSequence(EXTRA_TITLE)?.toString() ?: "No Title"
                val text = sbn.notification.extras.getCharSequence(EXTRA_TEXT)?.toString() ?: "No Text"
                ubiFormService.publishNotification(
                    sbn.packageName,
                    title,
                    text,
                    null
                )

                //ubiFormService.publishNotification("HELLO","EXTRA",null)
            }
        } else {
            Log.e("NOTIFICATION", "Service not bounded")
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