package com.traveling.travelingservice

import android.content.Intent
import android.os.Bundle
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import com.traveling.travelingservice.interfaces.CategoryActivity
import com.traveling.travelingservice.interfaces.MainForm
import com.traveling.travelingservice.ui.theme.MyLoginApplicationTheme
import com.traveling.travelingservice.ui.theme.MyMainApplicationTheme

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            MyMainApplicationTheme {
                MainForm { category ->
                    handleCategoryClick(category)
                }
            }
        }
    }

    private fun handleCategoryClick(category: String) {
        /*if (category == "Горные") {
            /*supportFragmentManager.beginTransaction()
                .replace(R.id.fragment_container, MountainFragment())
                .addToBackStack(null)
                .commit()*/
           // Toast.makeText(this, "Вы выбрали категорию: $category", Toast.LENGTH_SHORT).show()
        }*/
        val intent = Intent(this, CategoryActivity::class.java).apply {
            putExtra("category", category)
        }
        startActivity(intent)
    }
}

@Composable
fun Greeting(name: String, modifier: Modifier = Modifier) {
    Surface {
        Column(
            verticalArrangement = Arrangement.Center,
            horizontalAlignment = Alignment.CenterHorizontally,
            modifier = Modifier.fillMaxSize()
        ) {
            Text(
                text = "Hello $name!",
                modifier = modifier
            )
        }
    }
}

@Preview(showBackground = true, device = "id:Nexus One", showSystemUi = true)
@Composable
fun GreetingPreview() {
    MyLoginApplicationTheme {
        Greeting("Android")
    }
}

@Preview(showBackground = true, device = "id:Nexus One", showSystemUi = true)
@Composable
fun GreetingPreviewDark() {
    MyLoginApplicationTheme(darkTheme = true) {
        Greeting("Android")
    }
}