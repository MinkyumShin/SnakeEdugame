# Assignment — Linked-list-based Queue and OurSet

This small C++ project contains:

- `dnode.h` — doubly-linked node implementation
- `dlist_toolkit.h` — helper functions for doubly-linked lists
- `ourset.h` — a Set implemented on top of `dnode` + `dlist_toolkit`
- `queue.h` — a header-only FIFO `Queue` implemented using `dnode` (this was just updated to correctly allow duplicates and preserve size)
- `queue_test.cpp` — a small test program demonstrating `Queue` behaviour
- `main.cpp` — tests for `OurSet`

Build & run (quick):

```bash
cd /home/sminkyum/Assignment
# compile the queue test
g++ -std=c++17 -O2 -Wall -Wextra queue_test.cpp -o queue_test
./queue_test
```

How to publish to GitHub (summary):

1. Create a repository on GitHub (via website or `gh` CLI).
2. In this folder run:

```bash
# initialize a local repo (if you haven't yet)
git init --initial-branch=main
# add files and commit
git add .
git commit -m "Initial commit: Queue and OurSet implementation"
# add remote (replace URL with your repo URL)
git remote add origin https://github.com/<your-username>/<repo-name>.git
# push
git push -u origin main
```

Or, with GitHub CLI installed, you can run:

```bash
gh repo create <repo-name> --public --source=. --remote=origin --push
```

License: choose and add a LICENSE file if you want to publish publicly.
