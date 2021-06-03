import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

public class JsonParser {
    static char bracesOpen = '{';
    static char bracesClose = '}';
    static char bracketOpen = '[';
    static char bracketClose = ']';
    static char colon = ':';
    static char comma = ',';
    static char doubleQuote = '"';

    public enum TokenType {
        INIT(0), NORMAL_TOKEN(1), STRING(2), BRACESO(3),
        BRACESC(4), BRACKETO(5), BRACKETC(6), COLON(7),
        COMMA(8);
        TokenType (int v) {
            value = v;
        }
        private int value;
        public String toString() {
            switch (value) {
                case 0:
                    return "Token_Init";
                case 1:
                    return "Token_Normal";
                case 2:
                    return "Token_String";
                case 3:
                    return "Token_BraceOpen";
                case 4:
                    return "Token_BraceClose";
                case 5:
                    return "Token_BracketOpen";
                case 6:
                    return "Token_BracketClose";
                case 7:
                    return "Token_Colon";
                case 8:
                    return "Token_Comma";
            }
            return "Token_ERR";
        }
    }

    static class Pair<F, S> {
        public F first;
        public S second;
        Pair(F f, S s) {
            first = f;
            second = s;
        }
    }

    static Map<Character, TokenType> reservedMap = new HashMap<>();
    static {
        reservedMap.put(bracesOpen, TokenType.BRACESO);
        reservedMap.put(bracesClose, TokenType.BRACESC);
        reservedMap.put(bracketOpen, TokenType.BRACKETO);
        reservedMap.put(bracketClose, TokenType.BRACKETC);
        reservedMap.put(colon, TokenType.COLON);
        reservedMap.put(comma, TokenType.COMMA);
        reservedMap.put(doubleQuote, TokenType.STRING);
    }

    private static ArrayList<Pair<String, TokenType>> GetToken(String jsonStr) {
        ArrayList<Pair<String, TokenType>> ret = new ArrayList<>();
        int i = 0;
        boolean foundBeginner = false;
        TokenType type = TokenType.INIT;
        for (int j = 0; j < jsonStr.length(); ++j) {
            char curChar = jsonStr.charAt(j);
            if (Character.isWhitespace(curChar)) {
                if (foundBeginner) {
                    ret.add(new Pair<>(jsonStr.substring(i, j), TokenType.NORMAL_TOKEN));
                    foundBeginner = false;
                    type = TokenType.INIT;
                }
                continue;
            }
            // System.out.println("cur char: " + curChar);
            TokenType curType = reservedMap.get(curChar);
            if (curType == null) {
                if (!foundBeginner) {
                    i = j;
                    foundBeginner = true;
                    type = TokenType.NORMAL_TOKEN;
                }
                continue;
            }
            if (curType == TokenType.STRING) {
                if (type == TokenType.NORMAL_TOKEN) {
                    ret.add(new Pair<>(jsonStr.substring(i, j), TokenType.NORMAL_TOKEN));
                }
                if (j + 1 < jsonStr.length()) {
                    int idx = jsonStr.indexOf(doubleQuote, j + 1);
                    if (idx < 0) {
                        System.err.println("Error 3");
                        System.exit(3);
                    }
                    ret.add(new Pair<>(jsonStr.substring(j + 1, idx), TokenType.STRING));
                    j = idx;
                } else {
                    System.err.println("Error 2");
                    break;
                }
                type = TokenType.INIT;
                i = j;
            } else {
                if (type == TokenType.INIT) {
                    ret.add(new Pair<>(jsonStr.substring(j, j + 1), curType));
                } else {
                    i = j;
                }
            }
        }
        return ret;
    }

    private static Pair<HashMap<String, Object>, Integer> GetObject(ArrayList<Pair<String, TokenType>> tokens, int idx) {
        HashMap<String, Object> ret = new HashMap<>();
        // tokens.get(idx)
        if (idx >= tokens.size() - 1) {
            System.err.println("Error 5");
            System.exit(5);
        }
        while(tokens.get(++idx).second != TokenType.BRACESC) {
            Pair<String, TokenType> curToken = tokens.get(idx);
            System.out.println("cur token：" + curToken.first);
            if (curToken.second == TokenType.COMMA) { continue; }
            if (curToken.second != TokenType.STRING) {
                System.err.println("key should be string type, get " + curToken.second);
                System.exit(6);
            }
            String key = curToken.first;
            curToken = tokens.get(++idx);
            System.out.println("cur token：" + curToken.first);
            if (curToken.second != TokenType.COLON) {
                System.err.println("key should followed by colon, get" + curToken.second);
                System.exit(7);
            }
            curToken = tokens.get(++idx);
            System.out.println("cur token：" + curToken.first);
            if (curToken.second == TokenType.BRACESO) {
                Pair<HashMap<String, Object>, Integer> res = GetObject(tokens, idx);
                idx = res.second;
                ret.put(key, res.first);
            } else if (curToken.second == TokenType.STRING || curToken.second == TokenType.NORMAL_TOKEN) {
                ret.put(key, curToken.first);
            } else if (curToken.second == TokenType.BRACKETO) {
                Pair<ArrayList<Object>, Integer> res = GetArray(tokens, idx);
                idx = res.second;
                ret.put(key, res.first);
            } else {
                System.err.println("Wrong value type: " + curToken.second);
                System.exit(8);
            }
        }
        return new Pair<>(ret, idx);
    }

    private static Pair<ArrayList<Object>, Integer> GetArray(ArrayList<Pair<String, TokenType>> tokens, int idx) {
        ArrayList<Object> array = new ArrayList<>();
        if (idx >= tokens.size() - 1) {
            System.err.println("Error 9");
            System.exit(9);
        }
        Pair<String, TokenType> curToken = null;
        while((curToken = tokens.get(++idx)).second != TokenType.BRACKETC) {
            System.out.println("cur token：" + curToken.first);
            switch (curToken.second) {
                case COMMA:
                    continue;
                case STRING:
                case NORMAL_TOKEN:
                    array.add(curToken.first);
                    break;
                case BRACKETO:
                    Pair<ArrayList<Object>, Integer> res = GetArray(tokens, idx);
                    idx = res.second;
                    array.add(res.first);
                    break;
                case BRACESO:
                    Pair<HashMap<String, Object>, Integer> ores = GetObject(tokens, idx);
                    idx = ores.second;
                    array.add(ores.first);
                    break;
                default:
                    System.err.println("Array can not append type " + curToken.second);
                    System.exit(10);
            }
        }
        return new Pair<>(array, idx);
    }

    public static HashMap<String, Object> ParseJson(String strJson) {
        ArrayList<Pair<String, TokenType>> tokens = GetToken(strJson);
        Pair<HashMap<String, Object>, Integer> ret = GetObject(tokens, 0);
        return ret.first;
    }

    public static void main(String[] args) {
        String s = "{\n" +
                "    \"item_feature\": \n" +
                "      {\n" +
                "          \"101\": [ \n" +
                "                {\n" +
                "                    \"type\": \"int64\",\n" +
                "                    \"name\": \"100001\",\n" +
                "                    \"value\": 250\n" +
                "                },\n" +
                "                {\n" +
                "                    \"type\": \"int64\",\n" +
                "                    \"name\": \"100006\",\n" +
                "                    \"value\": 456\n" +
                "                },\n" +
                "                {\n" +
                "                    \"type\": \"int64\",\n" +
                "                    \"name\": \"100005\",\n" +
                "                    \"value\": 777\n" +
                "                },\n" +
                "                {\n" +
                "                    \"type\": \"float\",\n" +
                "                    \"name\": \"100017\",\n" +
                "                    \"value\": 135.1\n" +
                "                }\n" +
                "            ]\n" +
                "      ,\n" +
                "        \"102\": [ \n" +
                "            {\n" +
                "                \"type\": \"int64\",\n" +
                "                \"name\": \"100001\",\n" +
                "                \"value\": 251\n" +
                "            },\n" +
                "            {\n" +
                "                \"type\": \"int64\",\n" +
                "                \"name\": \"100006\",\n" +
                "                \"value\": 457\n" +
                "            },\n" +
                "            {\n" +
                "                \"type\": \"int64\",\n" +
                "                \"name\": \"100005\",\n" +
                "                \"value\": 788\n" +
                "            },\n" +
                "            {\n" +
                "                \"type\": \"float\",\n" +
                "                \"name\": \"100017\",\n" +
                "                \"value\": 133.1\n" +
                "            }\n" +
                "        ]\n" +
                "    },\n" +
                "\n" +
                "    \"common_feature\":\n" +
                "    [\n" +
                "        {\n" +
                "            \"type\": \"string\",\n" +
                "            \"name\": \"200001\",\n" +
                "            \"value\": \"beijing\"\n" +
                "        },\n" +
                "        {\n" +
                "            \"type\": \"string\",\n" +
                "            \"name\": \"200004\",\n" +
                "            \"value\": \"shanghai\"\n" +
                "        }    \n" +
                "    ]\n" +
                "}\n";
//        ArrayList<Pair<String, TokenType>> tokens = GetToken(s);
//        // for (Pair<String, TokenType> t : tokens) {
//        //     System.out.println(t.first + "\t " + t.second);
//        // }
//
//        Pair<HashMap<String, Object>, Integer> ret = GetObject(tokens, 0);
//        System.out.println(ret.second);
//        System.out.println(tokens.size());
//        HashMap<String, Object> theMap = ret.first;
        HashMap<String, Object> theMap = ParseJson(s);
        HashMap<String, Object> docs = (HashMap<String, Object>) theMap.getOrDefault("item_feature", null);
        System.out.println("docs is null:" + (docs == null));
        docs.forEach((k, v) -> {
            System.out.println("docid: " + k);
            ArrayList<HashMap<String, Object>> docFeats = (ArrayList<HashMap<String, Object>>) v;
            for (HashMap<String, Object> feat : docFeats) {
                // ArrayList<HashMap<String, Object>> doc = (ArrayList<HashMap<String, Object>>) docFeat;
                    System.out.println(feat.get("type"));
                    System.out.println(feat.get("name"));
                    System.out.println(feat.get("value"));
            }
        });

        ArrayList<HashMap<String, Object>> comm =
                (ArrayList<HashMap<String, Object>>) theMap.getOrDefault("common_feature", null);
        for (HashMap<String, Object> comFeat : comm) {
            System.out.println(comFeat.get("type"));
            System.out.println(comFeat.get("name"));
            System.out.println(comFeat.get("value"));
        }
        System.out.println("OK🎃💀😈");
    }
}
