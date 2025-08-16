package com.gm.objectdetectionwithtensorflowdemo

import android.content.pm.PackageManager
import android.graphics.Bitmap
import android.graphics.ImageDecoder
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.provider.MediaStore
import android.widget.ImageView
import androidx.activity.enableEdgeToEdge
import androidx.activity.result.PickVisualMediaRequest
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import androidx.cardview.widget.CardView
import androidx.core.content.FileProvider
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import com.gm.objectdetectionwithtensorflowdemo.mlhelper.Classifier
import java.io.File

class GrayscaleImageConversion : AppCompatActivity() {
    private lateinit var imageUri: Uri
    private lateinit var classifier: Classifier
    private lateinit var imgV: ImageView
    private lateinit var chooseBtn: CardView
    private lateinit var captureBtn: CardView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_grayscale_image_conversion)
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }
        takePermission()
        imgV = findViewById(R.id.imageView2)
        chooseBtn = findViewById(R.id.cardView2)
        captureBtn = findViewById(R.id.cardView3)

        val pickMedia =
            registerForActivityResult(ActivityResultContracts.PickVisualMedia()) { uri ->
                uri?.let {
                    createBitmapFromUriAndConvertGrayscale(uri)
                }
            }

        chooseBtn.setOnClickListener {
            pickMedia.launch(PickVisualMediaRequest(ActivityResultContracts.PickVisualMedia.ImageOnly))
        }

        imageUri = createImageUri()
        val takePicture = registerForActivityResult(ActivityResultContracts.TakePicture()) {
            createBitmapFromUriAndConvertGrayscale(imageUri)
        }


        captureBtn.setOnClickListener {
            takePicture.launch(imageUri)
        }
    }

    private fun convertBitIntoIntArray(bitmap:Bitmap,height: Int, width: Int):IntArray {
        val pixels = IntArray(height*width);
        bitmap.getPixels(pixels,0,width,0,0,width,height)
        return pixels
    }

    private fun createImageUri(): Uri {
        val image = File(applicationContext.filesDir, "camera_photo.png")

        return FileProvider.getUriForFile(
            applicationContext,
            "com.gm.basicopencvwithcamera.fileprovider",
            image
        )
    }

    fun takePermission() {
        if (checkSelfPermission(android.Manifest.permission.CAMERA) == PackageManager.PERMISSION_DENIED
        ) {
            val permission = arrayOf(
                android.Manifest.permission.CAMERA

            )
            requestPermissions(permission, 1122)
        }
    }

    fun createBitmapFromUriAndConvertGrayscale(uri:Uri) {
        var bitmap = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            ImageDecoder.decodeBitmap(
                ImageDecoder.createSource(
                    this@GrayscaleImageConversion.contentResolver,
                    uri
                )
            )
        } else {
            MediaStore.Images.Media.getBitmap(this.contentResolver,uri)
        }
        bitmap = bitmap.copy(Bitmap.Config.ARGB_8888,false)
        val height = bitmap.height
        val width = bitmap.width
        val pixels = convertBitIntoIntArray(bitmap, height, width)
        val arrBitmap=convertImageIntoGrayscale(pixels,height,width)
        bitmap = Bitmap.createBitmap(arrBitmap,width,height,Bitmap.Config.ARGB_8888)
        imgV.setImageBitmap(bitmap)
    }

    external fun convertImageIntoGrayscale(arrBitmap: IntArray,height:Int,width:Int):IntArray

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}