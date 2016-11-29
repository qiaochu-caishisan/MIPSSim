import java.io.*;
import java.util.*;

/**
 * Created by qiaochu on 16/9/21.
 */
public class disassemble {
    public static void main(String[] args) {
        byte[] byteBinary;//byte array store the binary code from the file
        char[][] handle;//a char array 二维数组 每32个构成一个一维数组 也就是一行一行的存起来
        //byteBinary = ReadFile("fibonacci_bin.bin");
        byteBinary = ReadFile("fibonacci_bin.bin");
        handle = distribute(byteBinary);
        String[] stringArray = new String[handle.length];
        String[] finalOutput;
        for(int i = 0; i<handle.length; i++){
            stringArray[i] =  new String(handle[i]);
        }

        finalOutput = assign(stringArray);
        outputFile(finalOutput);
        for(int i = 0; i<finalOutput.length; i++){
            System.out.println(finalOutput[i]);
        }
        //System.out.println(byteBinary[0]);



        //System.out.println(binaryString);//string contains all the binary code

        //System.out.println(Arrays.toString(ReadFile("/Users/qiaochu/Desktop/fibonacci_bin.bin")));

    }
//a function to read the binary codes from the files
    public static byte[] ReadFile(String fileName) {

        File file = new File(fileName);
        int length = (int)file.length();
        //byte[] byteArray;
        // if (file.exists()) {
        byte[] itemBuf;
        try {
            FileInputStream in = new FileInputStream(file);
            DataInputStream dis = new DataInputStream(in);

            itemBuf = new byte[length];


            //read the file until meet its end
            while (dis.available() != 0) {
                dis.read(itemBuf, 0, length);

            }

        } catch (IOException e) {
            e.printStackTrace();
            return null;
        } finally {
            //close
        }


        return itemBuf;



    }
    // a function used to write the output on the txt
    public static void outputFile(String[] outputArray){
        try{
            File myfile = new File("output.txt");
            if(!myfile.exists())
                myfile.createNewFile();

            FileWriter out = new FileWriter(myfile);
            for(int i = 0; i < outputArray.length; i++){
                out.write(outputArray[i] + "\r");
            }
            out.close();
        }catch (IOException e){
            e.printStackTrace();
        }
    }
    //a function used to distribute every 32 bit to handle[] and every bit to handle[][]
    public static char[][] distribute(byte[] abyteArray){
        String binaryString = new String();
        char[][] byteArray = new char[abyteArray.length/4][32];

        String ZERO = "00000000";
        for(int i = 0;i<abyteArray.length;i++){
            String test = Integer.toBinaryString(abyteArray[i]);
            if(test.length() > 8){
                test = test.substring(test.length() - 8);
            }else if(test.length() < 8){
                test = ZERO.substring(test.length()) +test;
            }
            binaryString = binaryString +test;

        }

        for(int n = 0; n < (binaryString.length()/32); n++){
            String tempString = binaryString.substring(32*n,32*(n+1));
            byteArray[n] = tempString.toCharArray();
        }
        return byteArray;
    }

    //a function to assign the assemble language to binary code in string array
    public static String[] assign(String[] astringArray){
        String[] orderPart = new String[6];
        String[] output = new String[astringArray.length];
        int[] address = new int [astringArray.length];
        int tempvalue =support(astringArray);


        for(int i = 0; i < astringArray.length; i++){
            address[i] = 600 + 4*i;
        }

        for(int i = 0; i <= tempvalue;i++){
            orderPart[0] = astringArray[i].substring(0,6);
            orderPart[1] = astringArray[i].substring(6,11);
            orderPart[2] = astringArray[i].substring(11,16);
            orderPart[3] = astringArray[i].substring(16,21);
            orderPart[4] = astringArray[i].substring(21,26);
            orderPart[5] = astringArray[i].substring(26,32);
            switch(orderPart[0]){
                case "101011":
                    output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                                orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "SW " + "R" +
                                Integer.valueOf(orderPart[2],2).toString() + ", "
                                + Integer.valueOf(orderPart[3] + orderPart[4] + orderPart[5],2).toString() +
                                "(" + "R" + Integer.valueOf(orderPart[1],2).toString() + ")";
                    break;


                case "100011":
                    output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                            orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "LW " + "R" + Integer.valueOf(orderPart[2],2).toString() + ", " +
                                 Integer.valueOf(orderPart[3] + orderPart[4] + orderPart[5],2).toString() +
                                 "(" + "R" + Integer.valueOf(orderPart[1],2).toString() + ")";
                    break;


                case "000010":
                    output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                            orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "J " + "#" + Integer.valueOf(orderPart[1].substring(2,5) + orderPart[2] + orderPart[3] +
                                    orderPart[4] + orderPart[5] + "00",2).toString();
                    break;


                case "000100":
                    output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                            orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "BEQ " + "R" + Integer.valueOf(orderPart[1],2).toString() + ", " +
                                "R" + Integer.valueOf(orderPart[2],2).toString() + ", " + "#" +
                            Integer.valueOf(orderPart[3].substring(2,5) + orderPart[4] + orderPart[5] + "00",2).toString();
                    break;


                case "000101":
                    output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                            orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "BNE " + "R" + Integer.valueOf(orderPart[1],2).toString() + ", " +
                                "R" + Integer.valueOf(orderPart[2],2).toString() + ", " + "#" +
                                Integer.valueOf(orderPart[3] + orderPart[4] + orderPart[5],2).toString();
                    break;


                case "000001":
                    if(orderPart[2].equals("00001"))
                        output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                                orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "BGEZ " + "R" + Integer.valueOf(orderPart[1],2).toString() + ", " +
                                "#" + Integer.valueOf(orderPart[3] + orderPart[4] + orderPart[5],2).toString();
                    else if(orderPart[2].equals("00000"))
                        output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                                orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "BLTZ " + "R" + Integer.valueOf(orderPart[1],2).toString() + ", " +
                                "#" + Integer.valueOf(orderPart[3] + orderPart[4] + orderPart[5],2).toString();
                    break;


                case "000111":
                    output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                            orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "BGTZ " + "R" + Integer.valueOf(orderPart[1],2).toString() + ", " +
                                "#" + Integer.valueOf(orderPart[3] + orderPart[4] + orderPart[5],2).toString();
                    break;


                case "000110":
                    output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                            orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "BLEZ " + "R" + Integer.valueOf(orderPart[1],2).toString() + ", " +
                                "#" + Integer.valueOf(orderPart[3] + orderPart[4] + orderPart[5],2).toString();
                    break;


                case "001000":
                    if(orderPart[3].substring(0,1).equals("0"))
                        output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                                orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "ADDI " + "R" + Integer.valueOf(orderPart[2],2).toString() + ", " +
                                    "R" + Integer.valueOf(orderPart[1],2).toString() + ", " + "#" +
                                    Integer.valueOf(orderPart[3] + orderPart[4] + orderPart[5],2).toString();
                    else if(orderPart[3].substring(0,1).equals("1"))
                        output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                                orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "ADDI " + "R" + Integer.valueOf(orderPart[2],2).toString() + ", " +
                                    "R" + Integer.valueOf(orderPart[1],2).toString() + ", " + "#" + "-" +
                                    Integer.valueOf(Integer.toBinaryString(~(Integer.valueOf(orderPart[3]
                                            + orderPart[4] + orderPart[5],2)) + 1).substring(16,32),2).toString();

                    break;

                case "001001":
                    output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                            orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "ADDIU " + "R" + Integer.valueOf(orderPart[2],2).toString() + ", " +
                                "R" + Integer.valueOf(orderPart[1],2).toString() + ", " + "#" +
                                Integer.valueOf(orderPart[3] + orderPart[4] + orderPart[5],2).toString();
                    break;


                case "000000":

                    if(orderPart[5].equals("001101"))
                        output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                                orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "BREAK";
                    else if(orderPart[5].equals("101010"))
                        output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                                orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "SLT " + "R" + Integer.valueOf(orderPart[3],2).toString() + ", " +
                                    "R" + Integer.valueOf(orderPart[1],2).toString() + ", " +
                                    "R" + Integer.valueOf(orderPart[2],2).toString();
                    else if(orderPart[5].equals("101011"))
                        output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                                orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "SLT " + "R" + Integer.valueOf(orderPart[3],2).toString() + ", " +
                                    "R" + Integer.valueOf(orderPart[1],2).toString() + ", " +
                                    "R" + Integer.valueOf(orderPart[2],2).toString();
                    else if(astringArray[i].equals("00000000000000000000000000000000"))
                        output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                                orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "NOP";
                    else if(orderPart[5].equals("000010"))
                        output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                                orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "SRL " + "R" + Integer.valueOf(orderPart[3],2).toString() + ", " +
                                "R" + Integer.valueOf(orderPart[2],2).toString() + ", " + "#" +
                                Integer.valueOf(orderPart[4],2).toString();
                    else if(orderPart[5].equals("000011"))
                        output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                                orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "SRA " + "R" + Integer.valueOf(orderPart[3],2).toString() + ", " +
                                "R" + Integer.valueOf(orderPart[2],2).toString() + ", " + "#" +
                                Integer.valueOf(orderPart[4],2).toString();
                    else if(orderPart[5].equals("100010"))
                        output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                                orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "SUB " + "R" + Integer.valueOf(orderPart[3],2).toString() + ", " +
                                "R" + Integer.valueOf(orderPart[1],2).toString() + ", " + "R" +
                                Integer.valueOf(orderPart[2],2).toString();
                    else if(orderPart[5].equals("100011"))
                        output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                                orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "SUBU " + "R" + Integer.valueOf(orderPart[3],2).toString() + ", " +
                                "R" + Integer.valueOf(orderPart[1],2).toString() + ", " + "R" +
                                Integer.valueOf(orderPart[2],2).toString();
                    else if(orderPart[5].equals("100000"))
                        output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                                orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "ADD " + "R" + Integer.valueOf(orderPart[3],2).toString() + ", " +
                                "R" + Integer.valueOf(orderPart[1],2).toString() + ", " + "R" +
                                Integer.valueOf(orderPart[2],2).toString();
                    else if(orderPart[5].equals("100001"))
                        output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                                orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "ADDU " + "R" + Integer.valueOf(orderPart[3],2).toString() + ", " +
                                "R" + Integer.valueOf(orderPart[1],2).toString() + ", " + "R" +
                                Integer.valueOf(orderPart[2],2).toString();
                    else if(orderPart[5].equals("100100"))
                        output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                                orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "AND " + "R" + Integer.valueOf(orderPart[3],2).toString() + ", " +
                                "R" + Integer.valueOf(orderPart[1],2).toString() + ", " + "R" +
                                Integer.valueOf(orderPart[2],2).toString();
                    else if(orderPart[5].equals("100101"))
                        output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                                orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "OR " + "R" + Integer.valueOf(orderPart[3],2).toString() + ", " +
                                "R" + Integer.valueOf(orderPart[1],2).toString() + ", " + "R" +
                                Integer.valueOf(orderPart[2],2).toString();
                    else if(orderPart[5].equals("100110"))
                        output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                                orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "XOR " + "R" + Integer.valueOf(orderPart[3],2).toString() + ", " +
                                "R" + Integer.valueOf(orderPart[1],2).toString() + ", " + "R" +
                                Integer.valueOf(orderPart[2],2).toString();
                    else if(orderPart[5].equals("100111"))
                        output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                                orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "NOR " + "R" + Integer.valueOf(orderPart[3],2).toString() + ", " +
                                "R" + Integer.valueOf(orderPart[1],2).toString() + ", " + "R" +
                                Integer.valueOf(orderPart[2],2).toString();
                    else if(orderPart[5].equals("000000"))
                            output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                            orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "SLL " + "R" + Integer.valueOf(orderPart[3],2).toString() + ", " +
                            "R" + Integer.valueOf(orderPart[2],2).toString() + ", " + "#" +
                            Integer.valueOf(orderPart[4],2).toString();

                    break;


                case "001010":
                    output[i] = orderPart[0] + " " + orderPart[1] + " " + orderPart[2] + " " + orderPart[3] + " " +
                            orderPart[4] + " " + orderPart[5] + " " + address[i] + " " + "ADDIU " + "R" + Integer.valueOf(orderPart[2],2).toString() + ", " +
                                "R" + Integer.valueOf(orderPart[1],2).toString() + ", " + "#" +
                                Integer.valueOf(orderPart[3] + orderPart[4] + orderPart[5],2).toString();
                    break;

            }

        }
        for(int i = tempvalue + 1; i < astringArray.length; i++){
            output[i] = astringArray[i] + " " + address[i] + " " + "0";
        }

        return output;

    }
    public static int support(String[] aastringArray){
        int atempvalue;
        for(int i = 0; i< aastringArray.length; i++){
            if (aastringArray[i].equals("00000000000000000000000000001101")) {
                atempvalue = i;
                return atempvalue;
            }
        }
        return 0;
    }

}
