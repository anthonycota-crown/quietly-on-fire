# ContextOS — Project Overview

## Client `package.json`

```json
{
  "name": "contextos-client",
  "version": "1.0.0",
  "type": "module",
  "scripts": {
    "dev": "vite",
    "build": "vite build",
    "preview": "vite preview"
  },
  "dependencies": {
    "d3": "^7.8.5",
    "lucide-react": "^0.294.0",
    "markdown-it": "^14.2.0",
    "prosemirror-commands": "^1.7.1",
    "prosemirror-example-setup": "^1.2.3",
    "prosemirror-history": "^1.5.0",
    "prosemirror-inputrules": "^1.5.1",
    "prosemirror-keymap": "^1.2.3",
    "prosemirror-model": "^1.25.9",
    "prosemirror-schema-basic": "^1.2.4",
    "prosemirror-state": "^1.4.4",
    "prosemirror-view": "^1.41.9",
    "react": "^18.2.0",
    "react-dom": "^18.2.0",
    "react-markdown": "^9.0.0",
    "react-router-dom": "^6.18.0",
    "react-syntax-highlighter": "^15.5.0",
    "recharts": "^3.8.1",
    "remark-gfm": "^4.0.0"
  },
  "devDependencies": {
    "@vitejs/plugin-react": "^4.1.0",
    "autoprefixer": "^10.4.0",
    "postcss": "^8.4.0",
    "tailwindcss": "^3.3.0",
    "vite": "^4.5.0"
  }
}
```

---

## Server `package.json`

```json
{
  "name": "contextos-server",
  "version": "1.0.0",
  "type": "commonjs",
  "scripts": {
    "start": "node index.js",
    "dev": "nodemon index.js",
    "test": "jest tests/"
  },
  "dependencies": {
    "@anthropic-ai/sdk": "^0.24.0",
    "@google/generative-ai": "^0.21.0",
    "@octokit/rest": "^20.1.1",
    "archiver": "^6.0.1",
    "axios": "^1.6.0",
    "better-sqlite3": "^9.4.0",
    "cheerio": "^1.0.0-rc.12",
    "chokidar": "^5.0.0",
    "clipboardy": "^3.0.0",
    "compromise": "^14.15.1",
    "cors": "^2.8.5",
    "csv-parse": "^5.5.0",
    "docx": "^8.5.0",
    "dotenv": "^16.3.1",
    "express": "^4.18.0",
    "express-rate-limit": "^8.5.2",
    "franc": "^6.2.0",
    "mammoth": "^1.6.0",
    "marked": "^9.1.0",
    "ml-distance": "^4.0.1",
    "multer": "^1.4.5-lts.1",
    "natural": "^8.1.1",
    "node-cron": "^3.0.3",
    "node-fetch": "^2.7.0",
    "nodejs-whisper": "^0.3.0",
    "openai": "^4.28.0",
    "pdf-parse": "^1.1.1",
    "puppeteer": "^21.5.0",
    "sentiment": "^5.0.2",
    "tesseract.js": "^5.0.4",
    "turndown": "^7.1.2",
    "uuid": "^9.0.0",
    "web-push": "^3.6.7",
    "xlsx": "^0.18.5",
    "youtube-transcript": "^1.2.1",
    "zod": "^3.25.76"
  },
  "devDependencies": {
    "jest": "^30.4.2",
    "nodemon": "^3.0.0"
  },
  "jest": {
    "testEnvironment": "node",
    "testTimeout": 30000
  }
}
```

---

## `src/` Directory Tree

**57 `.js` / `.jsx` files** (no `.ts`/`.tsx`)

```
src/
├── App.jsx
├── api/
│   └── client.js
├── components/
│   ├── chat/
│   │   ├── ActionBar.jsx
│   │   ├── ChatInput.jsx
│   │   ├── ChatMessage.jsx
│   │   ├── StreamingMessage.jsx
│   │   └── VizCard.jsx
│   ├── collectors/
│   │   ├── CodeCollector.jsx
│   │   ├── EmailCollector.jsx
│   │   ├── FileCollector.jsx
│   │   ├── PasteCollector.jsx
│   │   ├── UrlCollector.jsx
│   │   └── YoutubeCollector.jsx
│   ├── documents/
│   │   ├── DocumentCard.jsx
│   │   ├── DocumentPreview.jsx
│   │   └── DocumentTagEditor.jsx
│   ├── layout/
│   │   ├── Layout.jsx
│   │   └── Sidebar.jsx
│   ├── library/
│   │   ├── IntelligencePanel.jsx
│   │   ├── LibraryCard.jsx
│   │   ├── LibraryChat.jsx
│   │   └── LibraryDocumentPanel.jsx
│   ├── sideroom/
│   │   ├── AddZone.jsx
│   │   ├── OperationHistory.jsx
│   │   ├── SideRoom.jsx
│   │   ├── ToolsZone.jsx
│   │   └── tools/
│   │       ├── AuditResult.jsx
│   │       ├── DistillResult.jsx
│   │       ├── ExportPanel.jsx
│   │       ├── MapResult.jsx
│   │       └── OptimizeResult.jsx
│   └── ui/
│       ├── Badge.jsx
│       ├── Button.jsx
│       ├── MeetingBriefPanel.jsx
│       ├── Modal.jsx
│       ├── NotificationBell.jsx
│       ├── Spinner.jsx
│       ├── Toast.jsx
│       └── VoiceModal.jsx
├── hooks/
│   ├── useChat.js
│   ├── useDocuments.js
│   ├── useIntelligence.js
│   ├── useLibraries.js
│   ├── useNotices.js
│   └── useSearch.js
├── index.css
├── main.jsx
└── pages/
    ├── AmbientInbox.jsx
    ├── Collect.jsx
    ├── CollectType.jsx
    ├── Dashboard.jsx
    ├── Inbox.jsx
    ├── Library.jsx
    ├── PeoplePage.jsx
    ├── QuickCapture.jsx
    ├── Search.jsx
    ├── Settings.jsx
    └── WritingPage.jsx
```
