package com.example.ubiformskeletonkey

import android.app.ActionBar
import android.content.Intent
import android.os.Bundle
import android.widget.Button
import android.widget.LinearLayout

class ComponentList : GeneralConnectedActivity() {
    private var rdhUrl: String = ""
    override fun connectedToUbiForm() {
        updateComponentList()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_component_list)
        rdhUrl = intent.getStringExtra("url").toString()
    }

    private fun updateComponentList() {
        val container = findViewById<LinearLayout>(R.id.component_container)
        Thread {
            if (ubiformServiceBound) {
                container.post { container.removeAllViews() }
                val components: Array<String> = ubiFormService.getComponentsFromRDH(rdhUrl, this)
                if (!components.isEmpty()) {
                    for (componentId in components) {
                        val component = Button(this)
                        component.text = componentId
                        component.layoutParams = LinearLayout.LayoutParams(
                            LinearLayout.LayoutParams.MATCH_PARENT,
                            ActionBar.LayoutParams.WRAP_CONTENT
                        )
                        component.setOnClickListener {
                            val intent = Intent(it.context, SocketList::class.java)
                                .apply {
                                    putExtra("rdh", rdhUrl)
                                    putExtra("id", componentId)
                                }
                            startActivity(intent)
                        }

                        container.post { container.addView(component) }
                    }
                    updateMainOutput("Updated components for RDH")
                }
            }
        }.start()
    }
}