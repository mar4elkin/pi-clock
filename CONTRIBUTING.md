# Contributing to PI Clock

Thanks for your interest in contributing to PI Clock!

## Development workflow

All changes should be developed in a separate branch created from `master`.

Use the following branch naming convention:

`feat/<description>` — new functionality

`fix/<description>` — bug fixes

`refactor/<description>` — internal improvements

`docs/<description>` — documentation updates

Examples: `feat/timer-screen`, `fix/screen-navigation`, `docs/contributing`.

Keep each branch focused on one logical change. Open a Pull Request targeting `master` when the work is ready.

## Commit messages

This project follows [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/).

Format:

`<type>(<scope>): <description>`

The scope is optional.

Supported types:

- `feat` — introduce new functionality
- `fix` — fix a bug
- `refactor` — change internal code without changing behavior
- `docs` — update documentation
- `test` — add or update tests
- `build` — modify build configuration or dependencies
- `ci` — change CI workflows
- `chore` — maintenance tasks not covered by other types

Examples:

`feat(timer): add duration adjustment`

`fix(ui): prevent duplicate screen rendering`

`refactor(monitor): simplify CPU usage calculation`

`docs: add contribution guidelines`

`build: update CMake configuration`

Write descriptions in English, use the imperative mood, start with a lowercase verb, and omit the final period.

For changes that break compatibility, add `!` after the type or scope, and explain the breaking change in the commit body.

## Pull Requests

Use a descriptive title following Conventional Commits.

Explain what changed, why it was necessary, and how it was tested.

Prefer small, focused Pull Requests. Make sure the project builds successfully before merging.

Use **Squash and merge** for changes consisting of temporary or fixup commits. Preserve individual commits when they represent meaningful, independently useful changes.