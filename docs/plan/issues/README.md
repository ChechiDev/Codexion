# Issues lane

GitHub issues (bugs, refactors, small enhancements) are planned here, apart from
the phases and their progression, with the same gates. This folder also turns
the lane on: `make status` shows the shared issue queue from GitHub.

- `ISS-<n>-<slug>.md`: one header per issue, created by `make issue-start N=<n>`.
- `ISS-<n>-<slug>/ISS-<n>.<k>-<slug>.md`: one spec per issue subtask.

Commands and rules: `harness/WORKFLOW.md` ("Issues lane").
