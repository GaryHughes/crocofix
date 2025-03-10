cmake captures the test output which can make it hard to debug so do the following to run directly instead of via `make test`

```
/Users/geh/Developer/crocofix_python/build-Debug/libcrocofixpython_tests
export PYTHONPATH=../libcrocofixpython
ln -s ~/Developer/crocofix_python/libcrocofixpython_tests/session.py
/opt/homebrew/bin/python3.12 message.py
```