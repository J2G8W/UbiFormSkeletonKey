package com.example.ubiformskeletonkey

import android.app.ActionBar
import android.content.Intent
import android.os.Bundle
import android.view.View
import android.view.View.INVISIBLE
import android.view.View.VISIBLE
import android.widget.*
import androidx.core.app.NotificationCompat
import androidx.core.app.NotificationManagerCompat

class MainActivity : GeneralConnectedActivity() {
    override fun connectedToUbiForm() {
        updateRDHList()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        val optionSpinner = findViewById<Spinner>(R.id.component_action_choice)
        val textInput = findViewById<EditText>(R.id.input_rdh)
        optionSpinner.onItemSelectedListener = object : AdapterView.OnItemSelectedListener{
            override fun onItemSelected(parent: AdapterView<*>?, view: View?, position: Int, id: Long) {
                when(position){
                    6 -> textInput.visibility = VISIBLE
                    else -> {
                        textInput.visibility = INVISIBLE
                    }
                }
            }

            override fun onNothingSelected(parent: AdapterView<*>?) {
                textInput.visibility = INVISIBLE
            }
        }
    }

    override fun onResume() {
        super.onResume()
        updateRDHList()
    }

    private fun updateRDHList(){
        if(ubiformServiceBound) {
            val container = findViewById<LinearLayout>(R.id.rdh_container)
            container.post{container.removeAllViews()}
            val rdhs: Array<String> = ubiFormService.getRDHUrls()
            for (rdhText in rdhs) {
                val rdh = Button(this)
                rdh.text = rdhText
                rdh.layoutParams = LinearLayout.LayoutParams(
                    LinearLayout.LayoutParams.MATCH_PARENT,
                    ActionBar.LayoutParams.WRAP_CONTENT
                )
                rdh.setOnClickListener {
                    val intent = Intent(it.context,ComponentList::class.java)
                        .apply { putExtra("url",rdhText) }
                    startActivity(intent)
                }
                container.post{container.addView(rdh)}
            }
        }
    }

    fun doComponentAction(view: View){
        findViewById<TextView>(R.id.main_output).text = "Completing action"
        val textInput = findViewById<EditText>(R.id.input_rdh)
        Thread {
            val choice = findViewById<Spinner>(R.id.component_action_choice)
            when (choice.selectedItemId) {
                0L -> {
                    val builder = NotificationCompat.Builder(applicationContext, "TEST2")
                        .setSmallIcon(R.drawable.ic_launcher_background)
                        .setContentTitle("My notification")
                        .setContentText("Much longer text that cannot fit one line...")
                        .setStyle(
                            NotificationCompat.BigTextStyle()
                            .bigText("Much longer text that cannot fit one line..."))
                        .setPriority(NotificationCompat.PRIORITY_DEFAULT)

                    with(NotificationManagerCompat.from(applicationContext)){
                        notify(1002,builder.build())
                    }
                }//ubiFormService.updateManifestWithHubs(this)
                1L -> ubiFormService.deregisterFromAllHubs(this)
                2L -> ubiFormService.closeRDH(this)
                3L -> ubiFormService.openRDH(this)
                4L -> updateMainOutput("Component address: " + ubiFormService.getComponentAddress())
                5L -> updateMainOutput("Resource Discovery Hub Address: " + ubiFormService.getRdhAddress())
                6L -> ubiFormService.addRDH(textInput.text.toString(), this)
                else -> {
                    updateMainOutput("Not a valid action")
                }
            }
            updateRDHList()
        }.start()
    }

}