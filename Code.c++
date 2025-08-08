// Thales Mallmann Hunemeyer
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int vv = 2000, va = 1000, vvm = 3000;
int tela = -1, telam = -2, telaact = 5, estado = 0;
int senha1 = 0, senha2 = 0, senha3 = 0, senha4 = 1;
int tent1 = 0, tent2 = 0, tent3 = 0, tent4 = 0, senhapos = 0;
int newtent1 = 0, newtent2 = 0, newtent3 = 0, newtent4 = 0;
int newsenhapos = 0;

bool senha = false, acesso = false;
bool mostrarErro = false, sairconfirm = false, definindoSenha = false;

unsigned long tempoAnterior = 0, erroTempo = 0;
unsigned long debounceMais = 0, debounceMenos = 0, debounceDir = 0, debounceEsq = 0;
unsigned long ultimoInput = 0, tempoInatividade = 5000;

int telaSenha = -1;
int bl = 0;

bool mostrandoMsgSenhaAlterada = false;
unsigned long tempoMsgSenhaAlterada = 0;

bool mostrandoMsgTempoExpirado = false;
unsigned long tempoMsgTempoExpirado = 0;

// ======================== FUNÇÕES NOVAS ========================

void resetarTentativasSenha() {
  tent1 = tent2 = tent3 = tent4 = 0;
  senhapos = 0;
}

void resetarTelaInicial() {
  tela = -1;
  telam = -2;
}

void mensagemTemporaria(const char* msg, unsigned long& tempoMsg, bool& flag, int tempo = 2000) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(msg);
  tempoMsg = millis();
  flag = true;
}

void mostrarMensagemSenhaAlterada() {
  senha1 = newtent1;
  senha2 = newtent2;
  senha3 = newtent3;
  senha4 = newtent4;
  resetarTentativasSenha();
  resetarTelaInicial();
  definindoSenha = false;
  telaSenha = -1;
  acesso = false;
  mensagemTemporaria("Senha Alterada!", tempoMsgSenhaAlterada, mostrandoMsgSenhaAlterada, 1500);
}

void mostrarMensagemTempoExpirado() {
  resetarTentativasSenha();
  resetarTelaInicial();
  mensagemTemporaria("Tempo expirado!", tempoMsgTempoExpirado, mostrandoMsgTempoExpirado);
  acesso = false;
  definindoSenha = false;
  sairconfirm = false;
  senha = false;
}

// ===============================================================

void setup() {
  pinMode(8, OUTPUT); pinMode(7, OUTPUT); pinMode(6, OUTPUT);
  pinMode(9, INPUT); pinMode(10, INPUT); pinMode(13, INPUT);
  lcd.begin(16, 2);
  lcd.print("Sistema Semaforo");
  lcd.setCursor(0, 1);
  lcd.print("Thales Hunemeyer");
  delay(2000);
  lcd.clear();
  tempoAnterior = millis();
  ultimoInput = millis();
}

void nums() {
  lcd.clear();
  if (telaact < 1) telaact = 1;
  if (tela == 0) { vv = telaact * 1000; lcd.print("Verde: "); }
  if (tela == 1) { va = telaact * 1000; lcd.print("Amarelo: "); }
  if (tela == 2) { vvm = telaact * 1000; lcd.print("Vermelho: "); }
  lcd.print(telaact);
}

void exibirSenhaNaTela(const char* titulo, int d1, int d2, int d3, int d4, int pos) {
  lcd.setCursor(0, 0);
  lcd.print(titulo);
  lcd.setCursor(strlen(titulo), 1);
  lcd.print(d1); lcd.print(d2); lcd.print(d3); lcd.print(d4);
  lcd.setCursor(strlen(titulo) + pos, 1);
  lcd.print("^");
}

void mostrarMenuConfirmacao() {
  lcd.setCursor(0, 1);
  lcd.print("Sim 9  Nao A5");
}

void alteraNumeroSenha(int pos, int valor, bool novaSenha = false) {
  if (novaSenha) {
    if (pos == 0) newtent1 = (newtent1 + valor + 10) % 10;
    else if (pos == 1) newtent2 = (newtent2 + valor + 10) % 10;
    else if (pos == 2) newtent3 = (newtent3 + valor + 10) % 10;
    else if (pos == 3) newtent4 = (newtent4 + valor + 10) % 10;
  } else {
    if (pos == 0) tent1 = (tent1 + valor + 10) % 10;
    else if (pos == 1) tent2 = (tent2 + valor + 10) % 10;
    else if (pos == 2) tent3 = (tent3 + valor + 10) % 10;
    else if (pos == 3) tent4 = (tent4 + valor + 10) % 10;
  }
}

void atualizaSenhaInput(int &pos, int &dig1, int &dig2, int &dig3, int &dig4, bool novaSenha = false) {
  unsigned long agora = millis();

  if (digitalRead(9) == HIGH && agora - debounceMais > 250) {
    alteraNumeroSenha(pos, 1, novaSenha);
    ultimoInput = agora;
    debounceMais = agora;
  }
  if (analogRead(A5) > 100 && agora - debounceMenos > 250) {
    alteraNumeroSenha(pos, -1, novaSenha);
    ultimoInput = agora;
    debounceMenos = agora;
  }
  if (digitalRead(10) == HIGH && agora - debounceDir > 250) {
    pos = (pos + 1) % 4;
    ultimoInput = agora;
    debounceDir = agora;
  }
  if (digitalRead(13) == HIGH && agora - debounceEsq > 250) {
    if (!novaSenha) {
      if (dig1 == senha1 && dig2 == senha2 && dig3 == senha3 && dig4 == senha4) {
        acesso = true;
        tela = 0;
        telam = -1;
        lcd.clear();
      } else {
        mostrarErro = true;
        erroTempo = agora;
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("Senha Incorreta!");
      }
    } else {
      if (!mostrandoMsgSenhaAlterada) {
        mostrarMensagemSenhaAlterada();
      }
    }
    ultimoInput = agora;
    debounceEsq = agora;
  }
}

void verificarInatividade() {
  unsigned long agora = millis();
  
  if ((acesso || definindoSenha) && (agora - ultimoInput > tempoInatividade)) {
    mostrarMensagemTempoExpirado();
  }
}

void loop() {
  unsigned long agora = millis();

  // Verificar inatividade em todas as telas
  verificarInatividade();
  
  if (mostrandoMsgTempoExpirado) {
    if (agora - tempoMsgTempoExpirado >= 2000) {
      mostrandoMsgTempoExpirado = false;
      lcd.clear();
    }
    return;
  }

  switch (estado) {
    case 0:
      digitalWrite(8, HIGH); digitalWrite(7, LOW); digitalWrite(6, LOW);
      if (agora - tempoAnterior >= vv) { estado = 1; tempoAnterior = agora; }
      break;
    case 1:
      digitalWrite(8, LOW); digitalWrite(7, HIGH); digitalWrite(6, LOW);
      if (agora - tempoAnterior >= va) { estado = 2; tempoAnterior = agora; }
      break;
    case 2:
      digitalWrite(8, LOW); digitalWrite(7, LOW); digitalWrite(6, HIGH);
      if (agora - tempoAnterior >= vvm) { estado = 0; tempoAnterior = agora; }
      break;
  }

  if (mostrandoMsgSenhaAlterada) {
    if (agora - tempoMsgSenhaAlterada >= 1500) {
      lcd.clear();
      mostrandoMsgSenhaAlterada = false;
    }
    return;
  }

  if (!acesso) {
    exibirSenhaNaTela("Senha:", tent1, tent2, tent3, tent4, senhapos);
    atualizaSenhaInput(senhapos, tent1, tent2, tent3, tent4, false);

    if (mostrarErro && (agora - erroTempo > 2000)) {
      mostrarErro = false;
      lcd.clear();
    }
    return;
  }

  if (definindoSenha && telaSenha == 0) {
    exibirSenhaNaTela("Nova Senha:", newtent1, newtent2, newtent3, newtent4, newsenhapos);
    atualizaSenhaInput(newsenhapos, newtent1, newtent2, newtent3, newtent4, true);
    return;
  }

  if (!sairconfirm && !definindoSenha) {
    if (digitalRead(10) == HIGH && agora - debounceDir > 250) {
      tela++;
      ultimoInput = agora;
      debounceDir = agora;
    }
    if (digitalRead(13) == HIGH && agora - debounceEsq > 250) {
      tela--;
      ultimoInput = agora;
      debounceEsq = agora;
    }
  }

  if (tela > 4) tela = 0;
  if (tela < 0) tela = 4;

  if (tela != telam) {
    lcd.clear();
    lcd.setCursor(0, 0);
    switch (tela) {
      case 0: lcd.print("Verde: "); lcd.print(vv / 1000); telaact = vv / 1000; break;
      case 1: lcd.print("Amarelo: "); lcd.print(va / 1000); telaact = va / 1000; break;
      case 2: lcd.print("Vermelho: "); lcd.print(vvm / 1000); telaact = vvm / 1000; break;
      case 3: lcd.print("Sair do menu?"); sairconfirm = true; senha = false; break;
      case 4: lcd.print("Trocar Senha?"); senha = true; sairconfirm = false; break;
    }
    telam = tela;
    ultimoInput = agora; // Atualiza o tempo de inatividade ao mudar de tela
  }

  if (tela == 3 && sairconfirm) {
    mostrarMenuConfirmacao();
    if (digitalRead(9) == HIGH && agora - debounceMais > 250) {
      tela = 4; senha = true; sairconfirm = false; lcd.clear(); 
      ultimoInput = agora;
      debounceMais = agora;
    }
    if (analogRead(A5) > 100 && agora - debounceMenos > 250) {
      tela = 0; sairconfirm = false; lcd.clear(); 
      ultimoInput = agora;
      debounceMenos = agora;
    }
  }

  if (tela == 4 && senha) {
    mostrarMenuConfirmacao();
    if (digitalRead(9) == HIGH && agora - debounceMais > 250) {
      lcd.clear();
      definindoSenha = true; telaSenha = 0;
      newtent1 = senha1; newtent2 = senha2; newtent3 = senha3; newtent4 = senha4;
      newsenhapos = 0;
      senha = false;
      telam = -1;
      ultimoInput = agora;
      debounceMais = agora;
    }
    if (analogRead(A5) > 100 && agora - debounceMenos > 250) {
      lcd.clear();
      resetarTentativasSenha();
      senha = false;
      acesso = false;
      telam = -1;
      ultimoInput = agora;
      debounceMenos = agora;
    }
  }

  if (!sairconfirm && !definindoSenha) {
    if (digitalRead(9) == HIGH && agora - debounceMais > 250) {
      telaact++; nums(); 
      ultimoInput = agora;
      debounceMais = agora;
    }
    if (analogRead(A5) > 100 && agora - debounceMenos > 250) {
      telaact = max(telaact - 1, 1); nums(); 
      ultimoInput = agora;
      debounceMenos = agora;
    }
  }
}
