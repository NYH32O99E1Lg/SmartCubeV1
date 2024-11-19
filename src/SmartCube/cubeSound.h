void beep(int buzz) {
    tone(PIN_BUZZER, buzz, 100); // Generate a tone on the buzzer at the specified frequency for 100 ms
    delay(100);                  // Wait for the tone to finish playing
    noTone(PIN_BUZZER);          // Stop the tone on the buzzer
}