package com.gm.objectdetectionwithtensorflowdemo

import android.net.Uri
import android.os.Bundle
import android.util.Log
import android.widget.ImageView
import android.widget.TextView
import androidx.activity.enableEdgeToEdge
import androidx.activity.result.PickVisualMediaRequest
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import androidx.cardview.widget.CardView
import androidx.core.content.FileProvider
import androidx.core.graphics.drawable.toBitmap
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import com.gm.objectdetectionwithtensorflowdemo.mlhelper.Classifier
import java.io.File

class MainActivity : AppCompatActivity() {
    private lateinit var imageUri: Uri
    private lateinit var classifier: Classifier
    private lateinit var imgV: ImageView
    private lateinit var chooseBtn: CardView
    private lateinit var captureBtn: CardView
    private lateinit var resultTV: TextView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_main)
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }
        classifier = Classifier(assets, "model_tm.tflite", "labels.txt", 224)

        imgV = findViewById(R.id.imageView2)
        chooseBtn = findViewById(R.id.cardView2)
        captureBtn = findViewById(R.id.cardView3)
        resultTV = findViewById(R.id.textView)

        val pickMedia =
            registerForActivityResult(ActivityResultContracts.PickVisualMedia()) { uri ->
                if (uri != null) {
                    imgV.setImageURI(uri)
                    doInference()
                }
            }

        chooseBtn.setOnClickListener {
            pickMedia.launch(PickVisualMediaRequest(ActivityResultContracts.PickVisualMedia.ImageOnly))
        }

        imageUri = createImageUri()
        val takePicture = registerForActivityResult(ActivityResultContracts.TakePicture()) {
            imgV.setImageURI(imageUri)
            doInference()
        }


        captureBtn.setOnClickListener {
            takePicture.launch(imageUri)
        }
    }

    private fun doInference() {
        val result = classifier.recognizeImage(imgV.drawable.toBitmap())
        Log.d("Classifier", result.toString())
        resultTV.text = ""
        result.forEach { recognition ->
            resultTV.append(recognition.title + "   " + recognition.confidence + "\n")
        }
    }

    private fun createImageUri(): Uri {
        val image = File(applicationContext.filesDir, "camera_photo.png")

        return FileProvider.getUriForFile(
            applicationContext,
            "com.sasken.basicopencvwithcamera.fileprovider",
            image
        )
    }
}