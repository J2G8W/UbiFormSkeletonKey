package com.example.ubiformskeletonkey

import android.app.ActionBar
import android.os.Bundle
import android.provider.AlarmClock
import android.view.View
import android.widget.*

class MainActivity : GeneralConnectedActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
    }

    override fun onResume() {
        super.onResume()
        updateRDHList()
    }

    private fun updateRDHList(){
        if(mBound) {
            findViewById<LinearLayout>(R.id.rdh_container).removeAllViews()
            val rdhs: Array<String> = mService.getRDHUrls()
            for (rdhText in rdhs) {
                val rdh = Button(this)
                rdh.text = rdhText
                rdh.layoutParams = LinearLayout.LayoutParams(
                    LinearLayout.LayoutParams.MATCH_PARENT,
                    ActionBar.LayoutParams.WRAP_CONTENT
                )

                findViewById<LinearLayout>(R.id.rdh_container).addView(rdh)
            }
        }
    }

    fun addRDH(view: View){
        findViewById<TextView>(R.id.main_output).text = "Trying to add RDH"
        val rdhText = findViewById<EditText>(R.id.input_rdh)
        val url : String = rdhText.text.toString()
        if(mService.addRDH(url)){
            val rdh = Button(this)
            rdh.text = url
            rdh.layoutParams = LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                ActionBar.LayoutParams.WRAP_CONTENT
            )
            findViewById<LinearLayout>(R.id.rdh_container).addView(rdh)
            findViewById<TextView>(R.id.main_output).text = "Success registering with: $url"
            rdhText.text.clear()
        }else{
            findViewById<TextView>(R.id.main_output).text = "Error registering with: $url"
        }
    }

    fun doComponentAction(view: View){
        findViewById<TextView>(R.id.main_output).text = "Completing action"
        val choice = findViewById<Spinner>(R.id.component_action_choice)
        when(choice.selectedItemId){
            0L -> mService.updateManifestWithHubs()
            1L -> mService.deregisterFromAllHubs()
            2L -> mService.closeRDH()
            3L -> mService.openRDH()
            4L -> findViewById<TextView>(R.id.main_output).text = "Component address: " + mService.getComponentAddress()
            5L -> findViewById<TextView>(R.id.main_output).text = "Resource Discovery Hub Address:" + mService.getRdhAddress()
            else -> {
                findViewById<TextView>(R.id.main_output).text = "Not a valid action"
            }
        }
        if(choice.selectedItemId in 0L..3L){
            findViewById<TextView>(R.id.main_output).text = "Completed " + choice.selectedItem.toString()
        }
        updateRDHList()
    }

}