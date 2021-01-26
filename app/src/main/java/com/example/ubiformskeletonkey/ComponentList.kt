package com.example.ubiformskeletonkey

import android.app.ActionBar
import android.os.Bundle
import android.provider.AlarmClock
import android.widget.Button
import android.widget.LinearLayout
import android.widget.TextView

class ComponentList : GeneralConnectedActivity() {
    private var rdhUrl : String = ""
    override fun connectedToUbiForm() {
        updateComponentList()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_component_list)
        rdhUrl = intent.getStringExtra("url").toString()
    }
    private fun updateComponentList(){
        if(mBound) {
            findViewById<LinearLayout>(R.id.component_container).removeAllViews()
            val components: Array<String> = mService.getComponents(rdhUrl)
            if (components.isEmpty()) {
                findViewById<TextView>(R.id.main_output).text = "Could not get the components from this RDH"
            } else {
                for (componentId in components) {
                    val component = Button(this)
                    component.text = componentId
                    component.layoutParams = LinearLayout.LayoutParams(
                        LinearLayout.LayoutParams.MATCH_PARENT,
                        ActionBar.LayoutParams.WRAP_CONTENT
                    )

                    findViewById<LinearLayout>(R.id.component_container).addView(component)
                }
            }
        }
    }

}