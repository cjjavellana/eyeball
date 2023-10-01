# Eyeball
Your automated partner to help you scan (i.e. eyeball check) your application configuration files and compare it against known configuration to prevent an incorrect configuration (e.g. typo configuration) from ruining your friday evening, because real software developers push code to PROD on fridays.

## Usage
Prepare a master configuration file to serve as the basis for verification:  

master-config.yml
```yaml
dev: |
 ...
uat: |
 ...
pre-prod: |
 ...
prod: |
    spring.data.jpa.url=jdbc:mariadb://localhost:3306/db1
```
The master-config.yml will serve as your source of truth.

To use the default matcher pattern, run eyeball as follows:
```bash
$ eyeball -f subject.yml -c master-config.yml
```

To specify your own matcher pattern, run eyeball as follows:
```bash
$ eyeball -f subject.yml -c master-config.yml -p "^(jdbc:mariadb|https?)://(.*)(:\\d+)?(\/.*)?$" -p "^([a-zA-Z0-9.-])+(:\\d{2,4})$"
```

Where:  
- subject.yml is the configuration file to be verified
- master-config.yml is the source of truth
- patterns if not specified, will use the default pattern that comes with eyeball. Otherwise will use the user provided patterns to identify which configuration needs to be compared with master-config.yml


