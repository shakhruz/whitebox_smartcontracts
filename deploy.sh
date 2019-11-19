cleos wallet unlock --password <./wallet_password.pwd

# deploy contract
cleos -u $EOS_ENDPOINT set contract $KYLIN_TEST_ACCOUNT ./contracts/eos/whitebox -p $KYLIN_TEST_ACCOUNT@owner
