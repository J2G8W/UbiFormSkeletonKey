package com.example.ubiformskeletonkey

import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.os.IBinder
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity

open class GeneralConnectedActivity : AppCompatActivity() {
    protected lateinit var mService: UbiFormService
    protected var mBound: Boolean = false
    protected val connection = object : ServiceConnection {
        override fun onServiceConnected(className: ComponentName, service: IBinder) {
            // We've bound to LocalService, cast the IBinder and get LocalService instance
            val binder = service as UbiFormService.LocalBinder
            mService = binder.getService()
            mBound = true
        }

        override fun onServiceDisconnected(arg0: ComponentName) {
            mBound = false
        }
    }
    override fun onStart(){
        super.onStart()
        Intent(this, UbiFormService::class.java).also { intent ->
            bindService(intent, connection, Context.BIND_AUTO_CREATE)
        }
    }

    override fun onStop() {
        super.onStop()
        unbindService(connection)
        mBound = false
    }
}