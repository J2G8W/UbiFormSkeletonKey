package com.example.ubiformskeletonkey

import android.os.Bundle
import android.view.View
import android.view.View.INVISIBLE
import android.view.View.VISIBLE
import android.widget.*

class SocketList : GeneralConnectedActivity() {
    var rdhUrl: String = ""
    var componentId: String = ""
    var successfulConnection: Boolean = false
    var correctComponentUrl: String = ""

    override fun connectedToUbiForm() {
        Thread {
            correctComponentUrl = ubiFormService.getCorrectRemoteAddress(rdhUrl, componentId, this)
            successfulConnection = correctComponentUrl.startsWith("tcp")

            if (successfulConnection) {
                updateMainOutput("Connected to ${correctComponentUrl}")
                generateSocketList()
            }
        }.start()
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
                    1 -> shortInputOne.hint = "Socket ID"
                    2 -> shortInputOne.hint = "Endpoint Type"
                    4 -> shortInputOne.hint = "RDH address"
                    5 -> shortInputOne.hint = "RDH address"
                    6 -> {
                        longInput.hint = "Manifest input"
                        longInput.setText(findViewById<TextView>(R.id.component_manifest).text.toString())
                    }
                }
                when (position) {
                    1, 2, 4, 5 -> {
                        shortInputOne.visibility = VISIBLE
                        shortInputTwo.visibility = INVISIBLE
                        longInput.visibility = INVISIBLE
                    }
                    6 -> {
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
        }
    }

    fun socketAction(view: View) {
        updateMainOutput("Completing action")
        val shortInputOne = findViewById<EditText>(R.id.short_input_one)
        val shortInputTwo = findViewById<EditText>(R.id.short_input_two)
        val longInput = findViewById<EditText>(R.id.long_input)
        val choices = findViewById<Spinner>(R.id.socket_action_choice)
        val textInputOne = shortInputOne.text.toString()
        Thread {
            when (choices.selectedItemPosition) {
                0 -> updateMainOutput("No action taken")
                1 -> {
                    TODO()
                }
                2 -> ubiFormService.requestCloseSocketsOfType(
                    correctComponentUrl,
                    textInputOne,
                    this
                )
                3 -> ubiFormService.requestCreateRDH(correctComponentUrl, this)
                4 -> ubiFormService.requestAddRDH(
                    correctComponentUrl,
                    shortInputOne.text.toString(),
                    this
                )
                5 -> ubiFormService.requestRemoveRDH(
                    correctComponentUrl,
                    shortInputOne.text.toString(),
                    this
                )
                6 -> ubiFormService.requestChangeComponentManifest(
                    correctComponentUrl,
                    longInput.text.toString(),
                    this
                )
            }
            generateSocketList()
        }.start()

        shortInputOne.text.clear()
        shortInputTwo.text.clear()
        longInput.text.clear()
        choices.setSelection(0)
    }

    fun generateSocketList() {
        if (ubiformServiceBound) {
            val manifest: String =
                ubiFormService.requestComponentManifest(correctComponentUrl, this)
            val componentManifest = findViewById<TextView>(R.id.component_manifest)
            componentManifest.post { componentManifest.text = manifest }

            val listContainer = findViewById<LinearLayout>(R.id.socket_container)

            listContainer.post { listContainer.removeAllViews() }
            val values = ubiFormService.getSocketDescriptors(correctComponentUrl, this)
            if (values.isEmpty()) return
            for (socket in values) {
                val entry = TextView(this)
                entry.text = socket
                listContainer.post { listContainer.addView(entry) }
            }
        }
    }
}
