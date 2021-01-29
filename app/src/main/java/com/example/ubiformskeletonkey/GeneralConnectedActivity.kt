package com.example.ubiformskeletonkey

import android.app.NotificationChannel
import android.app.NotificationManager
import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.os.Build
import android.os.IBinder
import android.util.Log
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity

abstract class GeneralConnectedActivity : AppCompatActivity() {
    protected lateinit var ubiFormService: UbiFormService
    protected var ubiformServiceBound: Boolean = false
    protected val ubiformServiceConnection = object : ServiceConnection {
        override fun onServiceConnected(className: ComponentName, service: IBinder) {
            // We've bound to LocalService, cast the IBinder and get LocalService instance
            val binder = service as UbiFormService.LocalBinder
            ubiFormService = binder.getService()
            ubiformServiceBound = true
            connectedToUbiForm()
        }

        override fun onServiceDisconnected(arg0: ComponentName) {
            ubiformServiceBound = false
        }
    }

    protected val notificationPublisherConnection = object : ServiceConnection {
        override fun onServiceConnected(className: ComponentName, service: IBinder) {
            Log.d("NOTIFICATION", "Connected to Notification publisher")

            // Create the NotificationChannel, but only on API 26+ because
            // the NotificationChannel class is new and not in the support library
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                val name = getString(R.string.channel_name)
                val descriptionText = "Lorem ipsum Julian is a legend"
                val importance = NotificationManager.IMPORTANCE_DEFAULT
                val channel = NotificationChannel("TEST2", name, importance).apply {
                    description = descriptionText
                }
                // Register the channel with the system
                val notificationManager: NotificationManager =
                    getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
                notificationManager.createNotificationChannel(channel)
                Log.d("NOTIFICATION", "Created channel")
            }
        }

        override fun onServiceDisconnected(arg0: ComponentName) {
        }
    }


    override fun onStart() {
        super.onStart()
        Intent(this, UbiFormService::class.java).also { intent ->
            bindService(intent, ubiformServiceConnection, Context.BIND_AUTO_CREATE)
        }
        Intent(this, NotificationPublisher::class.java).also { intent ->
            bindService(intent, notificationPublisherConnection, Context.BIND_AUTO_CREATE)
        }
    }

    override fun onStop() {
        super.onStop()
        ubiformServiceBound = false
        unbindService(ubiformServiceConnection)
        unbindService(notificationPublisherConnection)
    }

    abstract fun connectedToUbiForm()

    fun updateMainOutput(text: String) {
        val out = findViewById<TextView>(R.id.main_output)
        out.post { out.text = text }
    }
}