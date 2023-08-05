
package ru.ostrov.zemu

import android.app.NativeActivity
import android.os.Bundle
import android.util.Log
import android.view.View

@Suppress("DEPRECATION")
class MainActivity : NativeActivity() {
    private fun setImmersiveSticky() {
        val decorView: View = window.decorView
        decorView.systemUiVisibility = (View.SYSTEM_UI_FLAG_FULLSCREEN
                or View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                or View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                or View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                or View.SYSTEM_UI_FLAG_LAYOUT_STABLE)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        setImmersiveSticky()
        val decorView = window.decorView
        decorView.setOnSystemUiVisibilityChangeListener { setImmersiveSticky() }
        super.onCreate(savedInstanceState)
    }

    override fun onResume() {
        setImmersiveSticky()
        super.onResume()
    }}