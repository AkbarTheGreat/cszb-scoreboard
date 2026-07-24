# Autoupdate Server

This service manages release version checks and binary downloads for the Scoreboard application. It serves Protocol Buffer payloads (`application/x-protobuf`) mapped from the project's [version_info.proto](file:///s:/src/cszb-scoreboard/version_info.proto) definition, with the Scoreboard client automatically falling back to GitHub Releases if this server goes offline.

---

## Folder Structure & How It Works

The server resolves releases dynamically by scanning the directory mounted at `/releases/`. It uses semantic version sorting to identify the latest version.

To make a version available, structure your files on the host as follows:

```text
/releases/
├── 1.0.0/                  <-- Semver version folder name
│   └── Win64/              <-- Platform name (e.g. Win64, MacOS)
│       └── cszb-scoreboard.exe
└── 1.1.0/
    └── Win64/
        └── cszb-scoreboard.exe
```

* **Latest Version Detection**: The server scans `/releases/`, filters out dotfiles/directories, sorts the remaining folders using semantic version comparison, and flags the highest version as `latest`.
* **Static Asset Serving**: All files inside `/releases/<version>/<platform>/` are exposed statically at the endpoint `/api/update_data/<version>/<platform>/<filename>`.

---

## Local Development & Testing

### Prerequisites
The server relies on [version_info.proto](file:///s:/src/cszb-scoreboard/version_info.proto) located in the repository root.

### Setup
1. Navigate to the app directory:
   ```bash
   cd autoupdate_server/app
   ```
2. Install dependencies:
   ```bash
   npm install
   ```
   *(Note: The `postinstall` hook in `package.json` will automatically copy the root `version_info.proto` to `./src/proto/version_info.proto` for compilation).*

3. Run the development server (with hot reloading):
   ```bash
   npm run dev
   ```

### Running Tests
The test suite utilizes mock release data located under `autoupdate_server/testdata/releases/`.
```bash
# Run tests with hot-reloading
npm test

# Run tests once (e.g., for CI/CD)
npm run test:no-reloading
```

---

## Production Deployment (Docker)

The production server runs inside a Docker container (Node-Alpine) and exposes port `3000`.

### 1. Build the Docker Image
Because the Docker context is scoped to the `autoupdate_server/` subdirectory, you must copy the root `version_info.proto` into the build context first:

**From the repository root:**
```bash
# Copy the single source of truth protobuf file
cp version_info.proto autoupdate_server/app/src/proto/

# Build the image using autoupdate_server as context
docker build -t cszb-autoupdate -f autoupdate_server/Dockerfile autoupdate_server
```

### 2. Run the Container
Run the container, mounting the host directory containing your release files to `/releases/`:

```bash
docker run -d \
  -p 80:3000 \
  -v /var/www/cszb-releases:/releases \
  --name autoupdate-server \
  --restart unless-stopped \
  cszb-autoupdate
```

---

## Releasing a New Version

To release a new update to clients:
1. Compile the Scoreboard client for the target platforms (e.g. `Win64` in Release mode).
2. Create the target version directory structure on your host machine under the mounted `/releases/` path (e.g., `/var/www/cszb-releases/1.2.0/Win64/`).
3. Upload the compiled executable/binary to that directory.
4. The server automatically starts serving the new version details at `/api/versions/latest` and update paths at `/api/versions/update_path/1.2.0/Win64/`. Existing clients will detect the new version on their next update check interval.
