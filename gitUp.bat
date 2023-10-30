echo off

set major=0
set minor=3
set sub=1
set build=0

git status
pause

git add *
git status
pause

echo Enter commit message
set /P "Message="

git commit -m "v%major%.%minor%.%sub% #%build% - %Message%"

git push https://github.com/dilara255/fViz2D.git

pause