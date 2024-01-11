## E2EE Chat Application

This hosts my final year project which was a e2e encrypted chat application, it was created to help solve the problem with censorship by allowing users to create and manage their own servers which users could connect to.

### Features
- High-performance backend
- Cross-platform web application that can run natively on mobile devices such a Android and iOS.
- Passwordless Authentication using [WebAuthN](https://developer.mozilla.org/en-US/docs/Web/API/Web_Authentication_API)
- Offline Notifications using [Push API](https://developer.mozilla.org/en-US/docs/Web/API/Push_API)
- Read Receipts
- Capacitor Library that trys to use the current native cryptography library or falls back to web [GitHub](https://github.com/Mr-Craig/capacitor-crypto)

### Technologies Used
- C++
- TypeScript
- [uWebSockets](https://github.com/uNetworking/uWebSockets)
- Ionic Framework
- SQLite
- Argon2
- OpenSSL

### Cryptography

Each chat uses their own key and is generated when the user accepts the chat request, the key exchange uses ECDH on the NIST P-256 curve and for encryption it uses AES-256-GCM.

### Disclaimer

This application has not been professional audited so I would not use this for sensitive communication.

### License

Copyright 2023 Alex Craig

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
