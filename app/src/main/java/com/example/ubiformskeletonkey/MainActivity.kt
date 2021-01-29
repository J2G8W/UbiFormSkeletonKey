package com.example.ubiformskeletonkey

import android.app.ActionBar
import android.content.Intent
import android.os.Bundle
import android.view.View
import android.view.View.INVISIBLE
import android.view.View.VISIBLE
import android.widget.*

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
        if(mBound) {
            val container = findViewById<LinearLayout>(R.id.rdh_container)
            container.post{container.removeAllViews()}
            val rdhs: Array<String> = mService.getRDHUrls()
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
                0L -> mService.updateManifestWithHubs(this)
                1L -> mService.deregisterFromAllHubs(this)
                2L -> mService.closeRDH(this)
                3L -> mService.openRDH(this)
                4L -> updateMainOutput("Component address: " + mService.getComponentAddress())
                5L -> updateMainOutput("Resource Discovery Hub Address: " + mService.getRdhAddress())
                6L -> mService.addRDH(textInput.text.toString(), this)
                else -> {
                    updateMainOutput("Not a valid action")
                }
            }
            updateRDHList()
        }.start()
    }

}