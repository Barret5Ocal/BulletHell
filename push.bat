@echo off 
git add -A
git commit -m %1
git push -u origin master