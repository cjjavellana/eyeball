# Eyeball
Your automated partner to help you scan (i.e. eyeball check) your application configuration files and compare it against known configuration to prevent an incorrect configuration (e.g. typo configuration) from ruining your friday evening, because real software developers push code to PROD on fridays.

## Usage
Inspired by liquibase convention mark up your application configuration's as follows:

src/main/resources/application.yml
```yaml
spring:
    data:
        jpa:
            # scanme env:prod 
            url: jdbc:mariadb://localhost:3306/db1 
```

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

Run eyeball as follows:
```bash
$ eyeball -f src/main/resources/application.yml -c master-config.yml
```


