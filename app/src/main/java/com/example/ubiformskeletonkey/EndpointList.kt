package com.example.ubiformskeletonkey

import android.os.Bundle
import android.view.View
import android.view.View.INVISIBLE
import android.view.View.VISIBLE
import android.widget.*

class EndpointList : GeneralConnectedActivity() {
    var rdhUrl: String = ""
    var componentId: String = ""
    var successfulConnection: Boolean = false
    var correctComponentUrl: String = ""

    override fun connectedToUbiForm() {
        updateMainOutput("Trying to connect")
        Thread {
            correctComponentUrl = ubiFormService.getCorrectRemoteAddress(rdhUrl, componentId, this)
            successfulConnection = correctComponentUrl.startsWith("tcp")

            if (successfulConnection) {
                updateMainOutput("Connected to ${correctComponentUrl}")
                generateEndpointList()
            }
        }.start()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_endpoint_list)
        rdhUrl = intent.getStringExtra("rdh").toString()
        componentId = intent.getStringExtra("id").toString()
        val optionSpinner = findViewById<Spinner>(R.id.socket_action_choice)

        val shortInputOne = findViewById<EditText>(R.id.short_input_one)
        val shortInputTwo = findViewById<EditText>(R.id.short_input_two)
        val shortInputThree = findViewById<EditText>(R.id.short_input_three)
        val shortInputFour = findViewById<EditText>(R.id.short_input_four)
        val longInput = findViewById<EditText>(R.id.long_input)

        optionSpinner.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(
                parent: AdapterView<*>?,
                view: View?,
                position: Int,
                id: Long
            ) {
                shortInputOne.text.clear()
                shortInputTwo.text.clear()
                longInput.text.clear()
                when (position) {
                    1 -> shortInputOne.hint = "Endpoint ID"
                    2 -> shortInputOne.hint = "Endpoint Type"
                    4 -> shortInputOne.hint = "RDH address"
                    5 -> shortInputOne.hint = "RDH address"
                    6 -> {
                        longInput.hint = "Manifest input"
                        longInput.setText(findViewById<TextView>(R.id.component_manifest).text.toString())
                    }
                    7 -> shortInputOne.hint = "Endpoint Type"
                    8 -> {
                        shortInputOne.hint = "Endpoint Type"
                        shortInputTwo.hint = "Dial URL"
                    }
                    9, 10 -> {
                        shortInputOne.hint = "Requester Endpoint Type"
                        shortInputTwo.hint = "Remote Endpoint Type"
                        shortInputThree.hint = "Remote address"
                        shortInputFour.hint = "Port"
                    }
                    11 -> {
                        shortInputOne.hint = "New Endpoint Type"
                        shortInputTwo.hint = "Third Party Component ID"
                        shortInputThree.hint = "Remote Endpoint Type"
                    }
                }
                when (position) {
                    1, 2, 4, 5, 7 -> {
                        shortInputOne.visibility = VISIBLE
                        shortInputTwo.visibility = INVISIBLE
                        shortInputThree.visibility = INVISIBLE
                        shortInputFour.visibility = INVISIBLE
                        longInput.visibility = INVISIBLE
                    }
                    6 -> {
                        shortInputOne.visibility = INVISIBLE
                        shortInputTwo.visibility = INVISIBLE
                        shortInputThree.visibility = INVISIBLE
                        shortInputFour.visibility = INVISIBLE
                        longInput.visibility = VISIBLE
                    }
                    8 -> {
                        shortInputOne.visibility = VISIBLE
                        shortInputTwo.visibility = VISIBLE
                        shortInputThree.visibility = INVISIBLE
                        shortInputFour.visibility = INVISIBLE
                        longInput.visibility = INVISIBLE
                    }
                    11 -> {
                        shortInputOne.visibility = VISIBLE
                        shortInputTwo.visibility = VISIBLE
                        shortInputThree.visibility = VISIBLE
                        shortInputFour.visibility = INVISIBLE
                        longInput.visibility = INVISIBLE
                    }
                    9, 10 -> {
                        shortInputOne.visibility = VISIBLE
                        shortInputTwo.visibility = VISIBLE
                        shortInputThree.visibility = VISIBLE
                        shortInputFour.visibility = VISIBLE
                        longInput.visibility = INVISIBLE
                    }
                    else -> {
                        shortInputOne.visibility = INVISIBLE
                        shortInputTwo.visibility = INVISIBLE
                        shortInputThree.visibility = INVISIBLE
                        shortInputFour.visibility = INVISIBLE
                        longInput.visibility = INVISIBLE
                    }
                }
            }

            override fun onNothingSelected(parent: AdapterView<*>?) {
                shortInputOne.visibility = INVISIBLE
                shortInputTwo.visibility = INVISIBLE
                shortInputThree.visibility = INVISIBLE
                shortInputFour.visibility = INVISIBLE
                longInput.visibility = INVISIBLE
            }
        }
    }

    fun endpointAction(view: View) {
        updateMainOutput("Completing action")
        val shortInputOne = findViewById<EditText>(R.id.short_input_one)
        val shortInputTwo = findViewById<EditText>(R.id.short_input_two)
        val shortInputThree = findViewById<EditText>(R.id.short_input_three)
        val shortInputFour = findViewById<EditText>(R.id.short_input_four)
        val longInput = findViewById<EditText>(R.id.long_input)
        val choices = findViewById<Spinner>(R.id.socket_action_choice)
        val textInputOne = shortInputOne.text.toString()
        val textInputTwo = shortInputTwo.text.toString()
        val textInputThree = shortInputThree.text.toString()
        val textInputFour = shortInputFour.text.toString()
        val textLongInput = longInput.text.toString()
        Thread {
            when (choices.selectedItemPosition) {
                0 -> updateMainOutput("No action taken")
                1 -> {
                    ubiFormService.requestCloseEndpointsOfID(
                        correctComponentUrl,
                        textInputOne,
                        this
                    )
                }
                2 -> ubiFormService.requestCloseEndpointsOfType(
                    correctComponentUrl,
                    textInputOne,
                    this
                )
                3 -> ubiFormService.requestCreateRDH(correctComponentUrl, this)
                4 -> ubiFormService.requestAddRDH(
                    correctComponentUrl,
                    textInputOne,
                    this
                )
                5 -> ubiFormService.requestRemoveRDH(
                    correctComponentUrl,
                    textInputOne,
                    this
                )
                6 -> ubiFormService.requestChangeComponentManifest(
                    correctComponentUrl,
                    textLongInput,
                    this
                )
                7 -> ubiFormService.requestCreateAndListen(correctComponentUrl, textInputOne, this)
                8 -> ubiFormService.requestCreateAndDial(
                    correctComponentUrl, textInputOne,
                    textInputTwo, this
                )
                9 -> {
                    try {
                        val portNum = textInputFour.toInt()
                        ubiFormService.request3rdPartyRemoteListenThenDial(
                            correctComponentUrl,
                            textInputOne,
                            textInputTwo,
                            textInputThree,
                            portNum,
                            this
                        )
                    } catch (e: NumberFormatException) {
                        updateMainOutput("Port given was not a number")
                    }
                }
                10 -> ubiFormService.request3rdPartyListenThenRemoteDial(
                    correctComponentUrl, textInputOne, textInputTwo,
                    "$textInputThree:$textInputFour", this
                )
                11 -> ubiFormService.addNewEndpointSchemaBasedOnOtherDevice(
                    correctComponentUrl,
                    textInputOne, rdhUrl, textInputTwo, textInputThree, this
                )
            }
            generateEndpointList()
        }.start()
        shortInputOne.text.clear()
        shortInputTwo.text.clear()
        shortInputThree.text.clear()
        shortInputFour.text.clear()
        longInput.text.clear()
        choices.setSelection(0)
    }

    fun generateEndpointList() {
        if (ubiformServiceBound) {
            val manifest: String =
                ubiFormService.requestComponentManifest(correctComponentUrl, this)
            val componentManifest = findViewById<TextView>(R.id.component_manifest)
            componentManifest.post { componentManifest.text = manifest }

            val listContainer = findViewById<LinearLayout>(R.id.socket_container)

            listContainer.post { listContainer.removeAllViews() }
            val values = ubiFormService.getEndpointDescriptors(correctComponentUrl, this)
            if (values.isEmpty()) return
            for (endpoint in values) {
                val entry = TextView(this)
                entry.text = endpoint
                listContainer.post { listContainer.addView(entry) }
            }
        }
    }
}
