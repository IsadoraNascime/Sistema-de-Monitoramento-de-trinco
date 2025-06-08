const int SENSOR_MAGNETICO = D2; // Pino do reed switch
const int LED_VERDE = D1;        // Pino do LED verde
const int LED_VERMELHO = D0;     // Pino do LED vermelho
const int BUZZER = D3;           // Pino do buzzer

// Configurações do buzzer
const int FREQUENCIA_ALERTA = 1000;      // Frequência do alarme em Hz
const int DURACAO_BEEP_ALARME = 200;     // Duração de cada beep de alarme
const int INTERVALO_BEEP = 200;          // Intervalo entre beeps
const unsigned long TEMPO_ALARME = 3000; // 3 segundos 

// Configurações do LED verde de confirmação
const int DURACAO_PISCA_VERDE = 3000;    // 3 segundos de piscada do led
const int INTERVALO_PISCA_VERDE = 200;   // Intervalo entre piscadas
const int TEMPO_ESPERA_BIP = 200; // Espera de 0.5 segundos antes do sinal de confirmação
unsigned long tempoSemIma = 0;
unsigned long tempoComIma = 0;
bool alarmeAtivo = false;
bool confirmacaoAtiva = false;
bool bipConfirmado = false;

void setup() {
  pinMode(SENSOR_MAGNETICO, INPUT_PULLUP);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int estadoSensor = digitalRead(SENSOR_MAGNETICO);

  if (estadoSensor == HIGH) { // Ímã presente
    if (!confirmacaoAtiva) {
      confirmacaoAtiva = true;
      tempoComIma = millis(); // Marca o tempo da detecção
      bipConfirmado = false;  // Reinicia controle de bip
      Serial.println("Ímã detectado: iniciando confirmação");
    }

    // Pisca o LED verde por 3 segundos
    if (millis() - tempoComIma <= DURACAO_PISCA_VERDE) {
      if ((millis() / INTERVALO_PISCA_VERDE) % 2 == 0) {
        digitalWrite(LED_VERDE, HIGH);
      } else {
        digitalWrite(LED_VERDE, LOW);
      }
    } else {
      digitalWrite(LED_VERDE, LOW);
    }

    // Executa o bip de confirmação após 1 segundo da detecção do ímã
    if (!bipConfirmado && millis() - tempoComIma >= TEMPO_ESPERA_BIP) {
      bipConfirmacao();
      bipConfirmado = true;
    }

    digitalWrite(LED_VERMELHO, LOW);
    noTone(BUZZER);
    tempoSemIma = 0;
    alarmeAtivo = false;

  } else { // Ímã ausente
    confirmacaoAtiva = false;
    bipConfirmado = false;
    digitalWrite(LED_VERDE, LOW);

    if (tempoSemIma == 0) {
      tempoSemIma = millis(); // Inicia contagem
    }

    if (millis() - tempoSemIma >= TEMPO_ALARME && !alarmeAtivo) {
      alarmeAtivo = true;
      Serial.println("ALARME: Ímã ausente há mais de 3 segundos!");
    }

    if (alarmeAtivo) {
      // Pisca o LED vermelho com buzzer
      digitalWrite(LED_VERMELHO, HIGH);
      tone(BUZZER, FREQUENCIA_ALERTA, DURACAO_BEEP_ALARME);
      delay(DURACAO_BEEP_ALARME);
      digitalWrite(LED_VERMELHO, LOW);
      noTone(BUZZER);
      delay(INTERVALO_BEEP);
    } else {
      Serial.print("Aguardando: ");
      Serial.print((millis() - tempoSemIma) / 1000);
      Serial.println("s sem ímã");
      delay(100);
    }
  }
}

// Função de bip de confirmação
void bipConfirmacao() {
  tone(BUZZER, 1500);
  delay(100);
  noTone(BUZZER);
  delay(100);
  tone(BUZZER, 1500);
  delay(100);
  noTone(BUZZER);
}
