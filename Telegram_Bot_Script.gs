// --- REPLACE THESE WITH YOUR OWN CREDENTIALS ---
var token = "YOUR_TELEGRAM_BOT_TOKEN_HERE"; 
var telegramUrl = "https://api.telegram.org/bot" + token;

// --- REPLACE WITH YOUR GOOGLE SHEET ID ---
var sheetId = "YOUR_GOOGLE_SHEET_ID_HERE"; 

function doPost(e) {
  if(typeof e !== 'undefined'){
    var contents = JSON.parse(e.postData.contents);
    var chat_id = contents.message.chat.id;
    var text = contents.message.text;

    if (text == "/start") {
      var sheet = SpreadsheetApp.openById(sheetId).getActiveSheet();
      var data = sheet.getDataRange().getValues();
      var exists = false;

      for (var i = 0; i < data.length; i++) {
        if (data[i][0] == chat_id) {
          exists = true;
          break;
        }
      }

      if (!exists) {
        sheet.appendRow([chat_id]);
         sendMessage(chat_id, "✅ PROTOCOL INITIALIZED: Connection Successful.\n\nWelcome to the Security Matrix. You are now fully synced with the central mainframe.\n\n🛡️ System Functions:\n- Real-time monitoring of vault access.\n- Instant alerts for unauthorized breach attempts.\n- Live updates on lockdown and self-destruct protocols.\n\nStandby for live updates...");
      } 
    }
  }
  return ContentService.createTextOutput("OK");
}

function doGet(e) {
  var message = e.parameter.message || "⚠️ TEST MESSAGE";
  var sheet = SpreadsheetApp.openById(sheetId).getActiveSheet();
  var data = sheet.getDataRange().getValues();

  for (var i = 0; i < data.length; i++) {
    var chat_id = data[i][0];
    if (chat_id) {
      sendMessage(chat_id, message);
    }
  }
  return ContentService.createTextOutput("Broadcast Success");
}

function sendMessage(chat_id, text) {
  var url = telegramUrl + "/sendMessage?chat_id=" + chat_id + "&text=" + encodeURIComponent(text);
  UrlFetchApp.fetch(url);
}