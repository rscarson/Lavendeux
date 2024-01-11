# Lavendeux [![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/rscarson/Lavendeux/master/LICENSE)

#### [Installer and binaries](http://rscarson.github.io/lavendeux/)

### Lavendeux enhances the clipboard
Lavendeux inlines calculating, programming and testing utilities into your favourite text editor.

Test the result of a remote API call, check the result of a regular expression and much more, all without needing to break your workflow.

Currently, Lavendeux is Windows compatible

Should in THEORY be Linux and OSX compatible as well, but I have no way to confirm that - if you are an OSX user interested in Lavendeux, please feel free to contact me.

#### How to use it
Highlight an expression, and press control space; Lavendeux will replace it with the answer.

Or, alternatively, turn auto copy/paste off in the menu, and CTRL-Space will instead solve any expressions in your clipboard.

It also supports a few command line options:

- config-dir: Change the configuration directory (defaults to ~/.lavendeux)
- debug: Change the logging level - useful for debugging issues (defaults to warning)

#### Extensions
Extend Lavendeux's featureset with [extensions](http://rscarson.github.io/Lavendeux/extensions). Extensions are written in javascript, sandboxed environment, with no network or host access.

#### What it can do
For list of supported operations and data types, please click [here](http://rscarson.github.io/lavendeux/docs)