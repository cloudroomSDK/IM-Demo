
import Foundation

public struct JsonTool {
    public static func fromJson<T: Decodable>(_ json: String, toClass: T.Type) -> T? {
       let decoder = JSONDecoder()
       do {
         let result = try decoder.decode(toClass, from: json.data(using: .utf8)!)
         return result
       } catch DecodingError.dataCorrupted(_) {
           print("dataCorrupted")
         return nil
       } catch let DecodingError.keyNotFound(key, context) {
           print("keyNotFound")
         return nil
       } catch let DecodingError.typeMismatch(type, context) {
           print("typeMismatch")
         return nil
       } catch DecodingError.valueNotFound(_, _) {
           print("valueNotFound")
         return nil
       } catch _ {
         return nil
       }
   }

    public static func toJson<T: Encodable>(fromObject: T) -> String {
        let encoder = JSONEncoder()
        do {
            let data = try encoder.encode(fromObject)
            guard var json = String(data: data, encoding: .utf8) else {
                fatalError("check your data is encodable from utf8!")
            }
            
            print("输出参数：\(json)")
            IMController.writeLog(content: "输出参数：\(json)")
            return json
        } catch let err {
            return ""
        }
    }
    
    public static func toMap<T: Encodable>(fromObject: T) -> [String: Any] {
        let encoder = JSONEncoder()
        do {
            let data = try encoder.encode(fromObject)
            guard let map = try? JSONSerialization.jsonObject(with: data, options: .mutableContainers) as? [String: Any] else {
                fatalError("check your data is encodable from utf8!")
            }
            
            print("输出参数：\(map)")
            IMController.writeLog(content: "输出参数：\(map)")
            return map
        } catch _ {
            return [:]
        }
     }
}
