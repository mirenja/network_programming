┌─────────────────────────────────────────────────────────────────────┐
│                      main() - Server Startup                         │
└────────────────────────────┬────────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────────┐
│         HttpServer::HttpServer(int port)                            │
│              - Initialize server_socket_                             │
│              - Create StateManager("chat_state.dat")                 │
└────────────────────────────┬────────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────────┐
│    ChatHandler::loadState(StateManager& state_manager)              │
│      ▼                                                              │
│    StateManager::loadMessages()                                      │
│      ▼                                                              │
│    Load from "chat_state.dat" file                                  │
│      ▼                                                              │
│    Deserialize each message line & populate messages_               │
└────────────────────────────┬────────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────────┐
│          server.run()                                                │
│    - Bind socket to port                                            │
│    - Listen for connections                                         │
└────────────────────────────┬────────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────────┐
│         While running: Accept client connections                    │
└────────────────────────────┬────────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────────┐
│    HttpServer::handleClient(Socket client_socket)                   │
│      - Receive HTTP request                                         │
│      - Parse request                                                │
└────────────────────────────┬────────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────────┐
│    HttpServer::routeRequest(HttpRequest request)                    │
│      - Match request path to registered routes                      │
└────────────────────────────┬────────────────────────────────────────┘
                             │
              ┌──────────────┼──────────────┐
              │              │              │
              ▼              ▼              ▼
    ┌──────────────┐  ┌──────────────┐  ┌───────────────────┐
    │  "/" route   │  │"/chat" route │  │"/chat/send" route │
    │(show home)   │  │(show chat)   │  │(POST message)     │
    └──────────────┘  └──────────────┘  └────────┬──────────┘
                                                 │
                                                 ▼
                                    ┌────────────────────────────┐
                                    │ ChatHandler::addMessage()  │
                                    │ (adds to messages_ vector) │
                                    └────────────────┬───────────┘
                                                    │
                                                    ▼
                                    ┌────────────────────────────┐
                                    │  HttpServer::saveState()   │
                                    │         (PERSISTENCE)      │
                                    └────────────────┬───────────┘
                                                    │
                                                    ▼
                      ┌─────────────────────────────────────────────┐
                      │  ChatHandler::saveState(StateManager&)      │
                      │         ▼                                   │
                      │  StateManager::saveMessages(messages)       │
                      │         ▼                                   │
                      │  Serialize each message                     │
                      │         ▼                                   │
                      │  Write to "chat_state.dat" file             │
                      └──────────────┬──────────────────────────────┘
                                     │
                                     ▼
                      ┌──────────────────────────────┐
                      │  Return response to client   │
                      │  (Redirect to /chat)         │
                      └──────────────────────────────┘


┌──────────────────────────────────────────────────────────────────┐
│           Graceful Shutdown (Ctrl+C)                             │
│              ▼                                                   │
│  HttpServer::stop()                                              │
│              ▼                                                   │
│  saveState() [Final persistence before shutdown]                │
│              ▼                                                   │
│  StateManager::saveMessages() to "chat_state.dat"               │
└──────────────────────────────────────────────────────────────────┘