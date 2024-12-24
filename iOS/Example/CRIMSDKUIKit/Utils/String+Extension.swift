
import Foundation
import CommonCrypto

extension String {
    func md5() -> String {
        let str = self.cString(using: String.Encoding.utf8)
        let strLen = CUnsignedInt(self.lengthOfBytes(using: String.Encoding.utf8))
        let digestLen = Int(CC_MD5_DIGEST_LENGTH)
        let result = UnsafeMutablePointer<CUnsignedChar>.allocate(capacity: digestLen)
        CC_MD5(str!, strLen, result)
        let hash = NSMutableString()
        for i in 0 ..< digestLen {
            hash.appendFormat("%02x", result[i])
        }
        result.deallocate()
        return String(format: hash as String)
    }
    
    func decodeBase64() -> String {
        var codeString = self
        let padding = codeString.count % 4
        if padding > 0 {
            codeString = codeString.padding(toLength: codeString.count + 4 - padding, withPad: "=", startingAt: 0)
        }
        if let data = Data(base64Encoded: codeString),
           let decodedString = String(data: data, encoding: .utf8) {
            return decodedString
        }
        return String()
    }
}
