ng build --prod
cd dist/
git init
git remote add azure 'https://p5datahub@datahubnode.scm.azurewebsites.net/DataHubNode.git'
git add -A 
git commit -m "deploy"
git push azure master --force