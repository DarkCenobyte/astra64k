# Put Astra on GitHub

## First upload

1. Create an empty public or private GitHub repository with any name you like.
2. Extract the archive and upload the **contents** of its folder to the
   repository root. `README.md`, `VERSION`, `CMakeLists.txt`, `src/`,
   `tools/`, `cmake/`, `licenses/`, and especially **`.github/`**
   should all be at the top level.
3. Include the hidden files `.github/`, `.gitignore`, and
   `.gitattributes`. Git normally includes them with `git add .`.

Example from the extracted folder:

```sh
git init -b main
git add .
git commit -m "Add Astra demoscene and cross-platform builds"
git remote add origin REPOSITORY_URL
git push -u origin main
```

Replace `REPOSITORY_URL` with the URL supplied by GitHub. No remote
repository was created or modified while preparing this archive.

## Get the first binaries

Open **Actions → Build, test and release**. Pushing to `main` starts
the builds. Once the jobs succeed, their files appear under **Artifacts**:

- `release-windows-x64`: EXE under 64,000 bytes, ZIP with notices, SHA-256;
- `release-linux-x64` and `release-linux-arm64`: native archives and SHA-256;
- `release-macos-arm64`: ARM64 `.app` bundle and SHA-256.

The Windows synthesis job runs after the EXE is built. Its startup log is
saved separately, including when the test fails. Large, reproducible test
WAV files are not published as deliverables.

You can rerun the builds without changes using **Run workflow**. Pull
requests also trigger checks, but do not publish a release.

## Publish a version

For the initial version supplied here, `VERSION` contains `5.2`:

```sh
git tag v5.2
git push origin v5.2
```

This push starts the builds and creates the release once all required jobs
have passed. Checksums and all four targets are checked before publication.
The workflow does not publish a partial release if any build fails.

For later versions, update `VERSION`, commit and push the change, then create
a new matching tag. Do not move a tag that has already been published.
GitHub will also offer the tag's sources as release downloads.

## Settings

- Enable GitHub Actions in the repository settings if they are disabled.
- The workflow uses only the `GITHUB_TOKEN` supplied by GitHub.
- `contents: read` is enough for build jobs; only the `release` job needs
  `contents: write`. Organization policy may restrict that permission.
- The selected runners are `ubuntu-24.04`, `ubuntu-24.04-arm`,
  `windows-2022`, and `macos-15` (ARM64). Their availability may change.
- Actions are pinned to commits; Dependabot proposes updates.
- The macOS signature is ad hoc, without Apple Developer ID or notarization.
- The repository name and owner are not hardcoded in the workflow.

[Official runner documentation](https://docs.github.com/en/actions/reference/runners/github-hosted-runners).

## Validation

The workflow is included in these sources; it has not yet run in your
repository. See [VALIDATION.md](VALIDATION.md) to distinguish completed
local checks from those awaiting the first GitHub run.
