package com.traveling.travelingservice.interfaces

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.google.accompanist.flowlayout.FlowRow

@Composable
fun MainForm(onCategoryClick: (String) -> Unit) {
//fun MainForm() {
    Surface(
        modifier = Modifier.fillMaxSize(),
        color = MaterialTheme.colorScheme.background
    ) {
        Column(
            verticalArrangement = Arrangement.Center,
            horizontalAlignment = Alignment.CenterHorizontally,
            modifier = Modifier
                .fillMaxSize()
                .padding(16.dp)
        ) {
            Text(
                text = "Группы путешествий",
                style = MaterialTheme.typography.headlineMedium,
                modifier = Modifier.padding(bottom = 16.dp)
            )

            val categories = listOf(
                "Горные", "Морские", "Природа", "Острова", "Города",
                "Деревни", "Озера", "Реки", "Моря", "Океаны",
                "Маленькие города", "Горные озера", "Пустыни", "Леса",
                "Пляжи", "Национальные парки", "Исторические места", "Культурные туры"
            )

            FlowRow(
                modifier = Modifier.fillMaxWidth(),
                mainAxisSpacing = 8.dp,
                crossAxisSpacing = 8.dp
            ) {
                categories.forEach { category ->
                    Button(
                        onClick = { onCategoryClick(category) },
                        //onClick = {},
                        shape = RoundedCornerShape(8.dp),
                        modifier = Modifier
                            .padding(vertical = 4.dp)
                    ) {
                        Text(text = category)
                    }
                }
            }
        }
    }
}
