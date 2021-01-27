package com.example.ubiformskeletonkey

import android.os.Bundle
import android.view.View
import android.view.View.INVISIBLE
import android.view.View.VISIBLE
import android.widget.AdapterView
import android.widget.EditText
import android.widget.Spinner
import android.widget.TextView
import org.w3c.dom.Text

class SocketList : GeneralConnectedActivity() {
    var rdhUrl : String = ""
    var componentId: String = ""
    var successfulConnection: Boolean = false
    var correctComponentUrl: String = ""

    override fun connectedToUbiForm() {
        val main_out = findViewById<TextView>(R.id.main_output)
        correctComponentUrl = mService.getCorrectRemoteAddress(rdhUrl, componentId,main_out)
        successfulConnection = correctComponentUrl.startsWith("tcp")

        if(successfulConnection) {
            main_out.text = "Connected to ${correctComponentUrl}"
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_socket_list)
        rdhUrl = intent.getStringExtra("rdh").toString()
        componentId = intent.getStringExtra("id").toString()
        val optionSpinner = findViewById<Spinner>(R.id.socket_action_choice)

        val shortInputOne = findViewById<EditText>(R.id.short_input_one)
        val shortInputTwo = findViewById<EditText>(R.id.short_input_two)
        val longInput = findViewById<EditText>(R.id.long_input)

        optionSpinner.setOnItemSelectedListener(object : AdapterView.OnItemSelectedListener{
            override fun onItemSelected(parent: AdapterView<*>?, view: View?, position: Int, id: Long) {
                shortInputOne.text.clear()
                shortInputTwo.text.clear()
                longInput.text.clear()
                when(position){
                    1 -> shortInputOne.hint = "Socket ID"
                    2 -> shortInputOne.hint = "Socket Hint"
                    4 -> shortInputOne.hint = "RDH address"
                    5 -> shortInputOne.hint = "RDH address"
                    6 -> longInput.hint = "Manifest input"
                }
                when(position){
                    1,2,4,5 -> {
                        shortInputOne.visibility = VISIBLE
                        shortInputTwo.visibility = INVISIBLE
                        longInput.visibility = INVISIBLE
                    }
                    6 ->{
                        shortInputOne.visibility = INVISIBLE
                        shortInputTwo.visibility = INVISIBLE
                        longInput.visibility = VISIBLE
                    }
                    else -> {
                        shortInputOne.visibility = INVISIBLE
                        shortInputTwo.visibility = INVISIBLE
                        longInput.visibility = INVISIBLE
                    }
                }
            }

            override fun onNothingSelected(parent: AdapterView<*>?) {
                shortInputOne.visibility = INVISIBLE
                shortInputTwo.visibility = INVISIBLE
                longInput.visibility = INVISIBLE
            }
        })
    }

    fun socketAction(view : View){
        val main_out = findViewById<TextView>(R.id.main_output)
        val shortInputOne = findViewById<EditText>(R.id.short_input_one)
        val shortInputTwo = findViewById<EditText>(R.id.short_input_two)
        val longInput = findViewById<EditText>(R.id.long_input)
        val choices = findViewById<Spinner>(R.id.socket_action_choice)
        when(choices.selectedItemPosition){
            0 -> main_out.text = "No action selected"
            1 -> {
                TODO()
            }
            2->{
                if(mService.requestCloseSocketsOfType(correctComponentUrl,shortInputOne.text.toString(), main_out)){
                    main_out.text = "Successfully closed socket"
                }
            }
            3->{
                if(mService.requestCreateRDH(correctComponentUrl,main_out)){
                    main_out.text = "Successfully created Resource Discovery Hub"
                }
            }
            4->{
                if(mService.requestAddRDH(correctComponentUrl, shortInputOne.text.toString(),main_out)){
                    main_out.text = "Successfully connected to ${shortInputOne.text}"
                }
            }
            5->{
                if(mService.requestRemoveRDH(correctComponentUrl, shortInputOne.text.toString(),main_out)){
                    main_out.text = "Successfully deregistered from ${shortInputOne.text}"
                }
            }
            6-> TODO()
        }
        shortInputOne.text.clear()
        shortInputTwo.text.clear()
        longInput.text.clear()
        choices.setSelection(0)
    }
}