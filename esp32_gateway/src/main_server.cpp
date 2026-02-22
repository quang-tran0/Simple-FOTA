#include "main_server.h"

const char *ssid = "MAY CAY HUY PHU";
const char *password = "12345678";

WebServer server(80);
File uploadFile;

void handleUpload() {
    HTTPUpload &upload = server.upload();

    if (upload.status == UPLOAD_FILE_START) {
        uploadFile = SPIFFS.open("/firmware.bin", FILE_WRITE);
        if (!uploadFile) {
            Serial.println("Cant open file /firmware.bin");
            return;
        }
        Serial.println("Upload...");
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (uploadFile)
            uploadFile.write(upload.buf, upload.currentSize);
    } else if (upload.status == UPLOAD_FILE_END) {
        if (uploadFile) {
            uploadFile.close();
            Serial.println("Upload done.");
        }
    }
}

// void main_server_task() {
//     server.on("/", HTTP_GET, []() {
//         server.send(200, "text/html",
//             R"rawliteral(
//                 <!DOCTYPE html>
//                 <html>
//                 <head>
//                 <title>Upload Firmware</title>
//                 <style>
//                     body {
//                     background-color: #f0f0f0;
//                     font-family: Arial, sans-serif;
//                     text-align: center;
//                     padding-top: 50px;
//                     }
//                     form {
//                     background: #fff;
//                     padding: 20px;
//                     border-radius: 10px;
//                     display: inline-block;
//                     box-shadow: 0 0 10px rgba(0,0,0,0.1);
//                     }
//                     input[type="file"] {
//                     margin-bottom: 15px;
//                     }
//                     input[type="submit"], a.button {
//                     background-color: #4CAF50;
//                     color: white;
//                     padding: 10px 20px;
//                     text-decoration: none;
//                     border: none;
//                     border-radius: 5px;
//                     cursor: pointer;
//                     }
//                     a.button {
//                     display: inline-block;
//                     margin-top: 15px;
//                     }
//                 </style>
//                 </head>
//                 <body>
//                 <form method="POST" action="/upload" enctype="multipart/form-data">
//                     <h2>OTA - STM32 - ESP32</h2>
//                     <input type="file" name="file" accept=".bin"><br>
//                     <input type="submit" value="Upload BIN">
//                     <br>
//                     <a href="/send" class="button">Send to STM32</a>
//                 </form>
//                 </body>
//                 </html>
//             )rawliteral"
//         );
//     });

//     server.on("/upload", HTTP_POST, []() { server.send(200, "text/plain", "Upload success!"); }, handleUpload);

//     server.on("/send", HTTP_GET, []() {
//         server.send(200, "text/plain", "Sending firmware.bin to STM32...");
//         sendFirmwareToSTM32();
//     });

//     server.begin();
// }

void main_server_task() {
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html",
R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>STM32 Firmware OTA</title>
    <style>
        :root {
            --primary: #2563eb;
            --success: #10b981;
            --bg: #f8fafc;
            --border: #cbd5e1;
            --text-main: #1e293b;
            --text-sub: #64748b;
        }

        body {
            font-family: 'Segoe UI', system-ui, sans-serif;
            background: var(--bg);
            display: flex;
            justify-content: center;
            align-items: center;
            /* Thay height thành min-height để không bị lệch khi nội dung dài ra */
            min-height: 100vh; 
            margin: 0;
        }

        .card {
            background: white;
            padding: 2.5rem;
            border-radius: 1.25rem;
            box-shadow: 0 10px 25px rgba(0, 0, 0, 0.05);
            width: 100%;
            max-width: 400px;
            text-align: center;
            border: 1px solid rgba(0,0,0,0.02);
        }

        h2 {
            color: var(--text-main);
            margin-top: 0;
            margin-bottom: 0.5rem;
            font-size: 1.75rem;
        }

        p {
            color: var(--text-sub);
            font-size: 0.95rem;
            margin-bottom: 2rem;
        }

        .file-input-wrapper {
            margin-bottom: 1.5rem;
            border: 2px dashed var(--border);
            border-radius: 0.75rem;
            padding: 1rem;
            transition: all 0.3s ease;
            background: #fff;
        }

        .file-input-wrapper:hover {
            border-color: var(--primary);
            background: #f1f5f9;
        }

        input[type="file"] {
            width: 100%;
            cursor: pointer;
            color: var(--text-sub);
        }

        button {
            background: var(--primary);
            color: white;
            border: none;
            padding: 1rem 2rem;
            border-radius: 0.75rem;
            font-weight: 600;
            font-size: 1rem;
            width: 100%;
            cursor: pointer;
            transition: all 0.2s ease;
            box-shadow: 0 4px 6px -1px rgba(37, 99, 235, 0.2);
        }

        button:hover {
            background: #1d4ed8;
            transform: translateY(-2px);
            box-shadow: 0 10px 15px -3px rgba(37, 99, 235, 0.3);
        }

        button:active {
            transform: translateY(0);
        }

        button:disabled {
            background: #cbd5e1;
            box-shadow: none;
            cursor: not-allowed;
            transform: none;
        }

        .progress-area {
            margin-top: 2rem;
            display: none;
        }

        .progress-bar {
            height: 10px;
            background: #e2e8f0;
            border-radius: 10px;
            overflow: hidden;
        }

        .progress-fill {
            height: 100%;
            background: var(--success);
            width: 0%;
            transition: width 0.4s cubic-bezier(0.4, 0, 0.2, 1);
        }

        #status {
            margin-top: 0.75rem;
            font-size: 0.9rem;
            font-weight: 600;
            color: var(--text-main);
        }
    </style>
</head>

<body>
    <div class="card">
        <h2>Firmware Update</h2>
        <p>Select .bin file to flash STM32</p>

        <div class="file-input-wrapper">
            <input type="file" id="fileInput" accept=".bin">
        </div>

        <button id="uploadBtn" onclick="startUpdate()">Flash Firmware</button>

        <div class="progress-area" id="progressArea">
            <div class="progress-bar">
                <div class="progress-fill" id="progressFill"></div>
            </div>
            <div id="status">Ready to upload...</div>
        </div>
    </div>

    <script>
        function startUpdate() {
            const fileInput = document.getElementById('fileInput');
            const btn = document.getElementById('uploadBtn');
            const status = document.getElementById('status');
            const progressArea = document.getElementById('progressArea');
            const fill = document.getElementById('progressFill');

            if (fileInput.files.length === 0) {
                alert("Please select a .bin file first!");
                return;
            }

            const file = fileInput.files[0];
            const formData = new FormData();
            formData.append('file', file);

            const xhr = new XMLHttpRequest();

            xhr.upload.addEventListener('progress', (e) => {
                if (e.lengthComputable) {
                    const percent = Math.round((e.loaded / e.total) * 100);
                    fill.style.width = percent + '%';
                    status.innerText = `Uploading to ESP32: ${percent}%`;
                }
            });

            xhr.onreadystatechange = function () {
                if (xhr.readyState == 4) {
                    if (xhr.status == 200) {
                        status.innerText = "Success! STM32 is flashing...";
                        status.style.color = "#10b981";
                        triggerStm32Send();
                    } else {
                        status.innerText = "Upload failed!";
                        status.style.color = "#ef4444";
                        btn.disabled = false;
                    }
                }
            };

            btn.disabled = true;
            progressArea.style.display = 'block';
            xhr.open('POST', '/upload', true);
            xhr.send(formData);
        }

        function triggerStm32Send() {
            const status = document.getElementById('status');
            fetch('/send')
                .then(response => response.text())
                .then(text => {
                    status.innerText = "All Done! System rebooting...";
                    setTimeout(() => { location.reload(); }, 3000);
                })
                .catch(err => {
                    status.innerText = "Error communication with STM32";
                    status.style.color = "#ef4444";
                });
        }
    </script>
</body>

</html>
        )rawliteral"
        );
    });

    server.on("/upload", HTTP_POST, []() { 
        server.send(200, "text/plain", "Upload success!"); 
    }, handleUpload);

    server.on("/send", HTTP_GET, []() {
        sendFirmwareToSTM32();
        server.send(200, "text/plain", "Flash Started");
    });

    server.begin();
}