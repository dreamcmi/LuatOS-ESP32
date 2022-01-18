local cryptoTest = {}

local tag = "cryptoTest"
local testData = string.rep("cryptoTest", 10)
local testKey = "123456"

local upper = string.upper

function cryptoTest.test()
    if crypto == nil then
        log.error(tag, "this fireware is not support crypto")
        return
    end
    log.info(tag, "START")

    assert(upper(crypto.md5(testData)) ==
               upper("5d0e9a7997bfa85e552af57710db3cb7"), tag .. ".md5 ERROR")
    assert(upper(crypto.hmac_md5(testData, testKey)) ==
               upper("5163edb731c5dae3be6e3f4acb00889b"),
           tag .. ".hmac_md5 ERROR")
    assert(upper(crypto.sha1(testData)) ==
               upper("3A2B9AEF7C5538B5250806A9D33543E9E3C6349F"),
           tag .. ".sha1 ERROR")
    assert(upper(crypto.hmac_sha1(testData, testKey)) ==
               upper("6763EB35CB4F437F45A3343B627FDDF73A510330"),
           tag .. ".hmac_sha1 ERROR")
    assert(upper(crypto.sha256(testData)) ==
               upper(
                   "5976342bb90317f5ab4341f44a7f1d1722bf6e99f3b932046a6b6d2b1b2da8e2"),
           tag .. ". ERROR")
    assert(upper(crypto.hmac_sha256(testData, testKey)) ==
               upper(
                   "640803BEB3069F60DCA65F515481ED9233E11E256C7E80BC7A0A51294AE3B829"),
           tag .. ".hmac_sha256 ERROR")
    assert(upper(crypto.sha512(testData)) == upper(
               "1cca08ee3c0bd67ce9a5d174aecf85f308c4797b1caabe6a6ce0f114246c5d0829ae6303e9d8406f46b6b91ae063325eb5dd390c5fc124c2324693f8d581257e"),
           tag .. ".sha512 ERROR")

    assert(upper(crypto.hmac_sha512(testData, testKey)) == upper(
               "8E582F9BFFA80B88E1E956C75B88826140FC5B5B2AAC910372A7A5170BB80A6F28B68C1831B76775F678C24BFD1ED7DFAF5C44B22EAB84849211C6107ED9F6E0"),
           tag .. ".hmac_sha512 ERROR")

    assert(crypto.cipher_decrypt("AES-128-CBC", "PKCS7",
                                 crypto.cipher_encrypt("AES-128-CBC", "PKCS7",
                                                       testData, testKey,
                                                       "1234567890666666"),
                                 testKey, "1234567890666666") == testData,
           tag .. ".cipher_decrypt ERROR")

    assert(crypto.crc16("MODBUS", testData) == 64642, tag .. ".crc16 ERROR")
    assert(crypto.crc16_modbus(testData) == 64642, tag .. ".crc16_modbus ERROR")
    assert(crypto.crc32("123456") == 158520161, tag .. ".crc32 ERROR")
    assert(crypto.crc8(testData) == 207, tag .. ".crc8 ERROR")

    for i = 1, 10 do
        local res = crypto.trng(1)
        local resNum = tonumber(res:toHex(), 16)
        assert(resNum >= 0 and resNum < 256, tag .. ".trng ERROR " .. resNum)
    end
    log.info(tag, "DONE")
end

return cryptoTest
