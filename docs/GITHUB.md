# Put Astra on GitHub

## First upload

1. Create an empty **public** GitHub repository with any name you like.
   Private/internal repositories need GitHub Enterprise Cloud for artifact
   attestations; tagged releases require them.
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

The Windows synthesis job runs after the EXE is built. Large, reproducible test
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
Each build signs provenance for its release files, and the release job
verifies their digests, source commit, tag, and workflow before publishing.
The workflow does not publish a partial release if any check fails.

Verify a downloaded EXE with the GitHub CLI, replacing `OWNER/REPO` with
the repository used for the release:

```sh
gh attestation verify Astra-v5.2-windows-x64.exe \
  --repo OWNER/REPO --source-ref refs/tags/v5.2 \
  --signer-workflow OWNER/REPO/.github/workflows/build-release.yml
```

You can also check other downloaded release files this way. Adding
`--source-digest EXPECTED_COMMIT_SHA` binds the result to a previously
verified commit. Attestations are stored by GitHub, not inside the EXE.

For later versions, update `VERSION`, commit and push the change, then create
a new matching tag. Do not move a tag that has already been published.
GitHub will also offer the tag's sources as release downloads.

## Settings

- Enable GitHub Actions in the repository settings if they are disabled.
- The workflow uses only the `GITHUB_TOKEN` supplied by GitHub.
- On branch and tag pushes and manual runs, the Windows, Linux, and macOS
  build jobs use `contents: read`, `id-token: write`, and
  `attestations: write` to sign
  their own release files. Only the `release` job has `contents: write`
  (plus `attestations: read` to verify them). Organization policy may
  restrict these permissions.
- The selected runners are `ubuntu-24.04`, `ubuntu-24.04-arm`,
  `windows-2022`, and `macos-15` (ARM64). Their availability may change.
- Actions, including `actions/attest`, are pinned to commits; Dependabot
  proposes updates.
- The macOS signature is ad hoc, without Apple Developer ID or notarization.
- The repository name and owner are not hardcoded in the workflow.

[Official runner documentation](https://docs.github.com/en/actions/reference/runners/github-hosted-runners).

## Validation

The workflow is included in these sources; it has not yet run in your
repository. See [VALIDATION.md](VALIDATION.md) to distinguish completed
local checks from those awaiting the first GitHub run.
